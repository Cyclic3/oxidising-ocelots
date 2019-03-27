try:
	from flask import Flask, abort, render_template, redirect, make_response, url_for, flash, request, escape
	from flask_socketio import SocketIO, join_room, leave_room

except:
	while True:
		input("You are missing dependencies. Please use pip to install the following \n Flask \n Flask-SocketIO==3.1.2")
import re, json, time
from threading import Thread
rooms = {}

import OcelotManager


oldList = activeList = []
USER_TIMEOUT = 10



def checkAlive(sleeper):
	time.sleep(sleeper)
	global activeList, oldList


	print (oldList)
	print (activeList)


	socketio.emit('AreYouAlive')

	print ("Poll sent")
		
	time.sleep(sleeper)

	print (oldList)
	print (activeList)	

	oldList = list(set(oldList))
	activeList = list(set(activeList))


	print (oldList)
	print (activeList)	


	if len(oldList) == 0:
		joinedUsers = activeList
		leftUsers = []
	elif len(activeList)  == 0:
		leftUsers = oldList
		joinedUsers = []

	else:
		print ('new thing')
		leftUsers = list(set(oldList)-set(activeList))
		joinedUsers = list(set(activeList)-set(oldList))

	if not leftUsers == []:
		for room in rooms:
			for user in oldList:
				if user in rooms[room].activeRoomUsers:
					rooms[room].RemoveActiveUserFromRoom(user)

	oldList = activeList
	activeList = []
	return('New Users:' +str(joinedUsers)+ '.  users who have left:' +str(leftUsers))


def checkAliveCycle(debug):
	global activeList, oldList
	if debug:
		print('auto polling not active while in debug mode. Please run server with debug = False')
		return
	activeList = []
	oldList = []
	while True:
		print (checkAlive(USER_TIMEOUT))



app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 300
socketio = SocketIO(app)


gameChatCommands = {
'/cardlist': 'Show all cards',
'/takecard': 'Take a card from stack and finish your turn',
'/playcard *card*': 'Play the card that you have chosen'
}



@app.route("/test/<cid>/<message>")
def tester1234(cid, message):

	jsonData = {
	'user_name': 'admin',
	'message': message+cid
	}
	socketio.emit('MessageResponse', jsonData, callback=messageReceived, room=cid)
	return ('yeet')

@app.route("/", methods = ["GET","POST"])
def home():
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())

	if (request.remote_addr.startswith('127') or request.remote_addr == '::1'):
		printRef = True
	else:
		printRef = False

	if request.method == 'GET':
		if not userData[0] and not userData[1]:
			return render_template("Home.html", userData = userData, askUsername = True, sidebar = True, printRef = True)
		else:
			try:
				int(userData[1])
			except:
				return redirect('refreshID')
			return render_template("Home.html", userData = userData, askUsername = False, sidebar = True, printRef = True)


	if request.method == 'POST':

		if request.form['mode'] == 'userjoin':
			resp = make_response(redirect(url_for('home')))
			resp.set_cookie('username', sanitiseInput(request.form['username']))
			resp.set_cookie('userID', generateID(0xFFFFFFFF))
			return resp

		elif request.form['mode'] == 'roomref':
			room_ref = request.form['room_ref']
			if not room_ref in rooms:
				flash ('That room cannot be found.', 'warning')
				return redirect(url_for('home'))
			else:
				return redirect(url_for('room', roomRefrence=room_ref))



@app.route("/refreshID", methods = ["GET","POST"])
def GetMeANewID():

	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')))

	if not userData[0] and not userData[1]:
		flash("You have not provided a username yet", 'warning')
		return redirect(url_for('home'))

	if request.method == 'GET':
		resp = make_response(redirect(url_for('home')))
	elif request.method == 'POST':
		resp = make_response(redirect(url_for('home')))
	resp.set_cookie('username', userData[0])
	resp.set_cookie('userID', generateID(0xFFFFFFFF))

	return resp


@app.route("/create_room", methods = ['GET', 'POST'])
def create_room_page():
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())

	if not (request.remote_addr.startswith('127') or request.remote_addr == '::1'):
		abort(403)

	if request.method == 'GET':
		return (render_template("CreateRoom.html", userData = userData))
	elif request.method == 'POST':
		roomIDCreated = (game_room(sanitiseInput(request.form['roomName'])).roomID)
		flash(("Room has been created. Your roomID is " + str(roomIDCreated)), 'success')
		return redirect(url_for('home'))

@app.route("/room/<roomRefrence>")
def room(roomRefrence):
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())

	if not userData[0] and not userData[1]:
		flash("You have not provided a username yet", 'warning')
		return redirect(url_for('home'))	


	if not roomRefrence in rooms:
		flash('That room cannot be found','warning')
		return redirect(url_for('home'))
	

	if rooms[roomRefrence].activeGame and (not userData[1] in rooms[roomRefrence].allactiveRoomUsers):
		print (userData)
		flash("This game is now active and you cannot join anymore", 'warning')
		return redirect(url_for('home'))


	for room in rooms:
		print (rooms[room].activeRoomUsers)
		if userData[1] in rooms[room].activeRoomUsers and not debug:
			flash("You appear to already be in a room. Perhaps you are in another tab? If you have just left a game, please wait upto 10 seconds before trying to connect", 'warning')
			return redirect(url_for('home'))
	return (render_template('room.html', userData = userData, helpbar = True, commands=gameChatCommands))


########################################################################################################################

def messageReceived(methods=['GET', 'POST']):
    print('message was received!!!')


@socketio.on('IAmAlive')
def amAliveNotification():
	global activeList
	activeList.append(request.cookies.get('userID'))


@socketio.on('connect')
def on_connect():
	pass



@socketio.on('registerToRoom')
def regsiterToRoom(data, methods=['GET', 'POST']):
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())
	activeList.append(data['userID'])

	lis = (request.sid)

	join_room(data['roomID'])

	roomRefrence = (data['roomID'])
	currentRoom = rooms.get(roomRefrence)
	currentRoom.activeRoomUsers[str(userData[1])] = (lis)

	jsonData = {
	'user_name': sanitiseInput(request.cookies.get('username')),
	'message': '<b><i>User Connected</i></b>'
	}

	
	socketio.emit('MessageResponse', jsonData, callback=messageReceived, room=data['roomID'])


@socketio.on('ActionHappened')
def nonUserAction(jsonData, methods=['GET', 'POST']):

	print (jsonData)

	if jsonData['message'].lower() == '/cardlist':
		jsonResponse = {
		'user_name': 'server',
		'message': 'would get a list of all cards here'
		}

	elif jsonData['message'].lower() == '/takecard':
		rooms[jsonData['roomID']].dumpAndSend()
		jsonResponse = {
		'user_name': 'server',
		'message': 'card taken'
		}

	elif jsonData['message'][:9].lower() == '/playcard':
		jsonResponse = {
		'user_name': 'server',
		'message': 'card played'
		}

	socketio.emit('MessageResponse', jsonResponse,  room=jsonData['roomID'])





@socketio.on('userAction')
def userAction(jsonData, methods=['GET', 'POST']):
	
	if jsonData['action'] == 'VoteToStart':
		print (jsonData['action'])
		rooms[jsonData['roomID']].startGame(jsonData['roomID'], jsonData['userID'])
	else:
		print ('INVALID ACTION')

########################################################################################################################





########################################################################################################################

def sanitiseInput(input):
	if input:
		output = escape(input)
		#output = (input.replace('script', 'scriρt')).replace('onload', 'onløad')
		#output = re.sub(r'[^\w\s]', '', input)
		return output
	else:
		return input



def generateID(val):
	import random
	return str(random.randint(0, val))


def listRooms():
	a = []
	for i in rooms:
		i = (rooms[i])
		a.append([i.roomName, i.roomID, len(i.activeRoomUsers), i.activeGame])
	return a


class game_room:
	def __init__(self, roomName):
		self.roomName = roomName
		self.roomID = hex(int(generateID(0xFFFFFFFF)))[2:]
		self.activeRoomUsers = {}
		self.allactiveRoomUsers = {}
		self.activeGame = False
		self.AddToActiveRooms()
		self.voteToStart = []

	def AddToActiveRooms(self):
		rooms.update({self.roomID: self})

	def RemoveFromActiveRooms(self):
		del rooms[self.roomID]

	def AddUserToRoom(self, userID):
		self.activeRoomUsers(userID)

	def RemoveActiveUserFromRoom(self, userID):
		del self.activeRoomUsers[userID]

	def startGame(self, room, userID):
		self.voteToStart.append(userID)

		if (len(self.activeRoomUsers)) <2:
			socketio.emit('MessageResponse', {'user_name': 'Server', 'message': '<b><i>Not enough users to start game</i></b>'}, room=self.roomID)
			if debug:
				socketio.emit('MessageResponse', {'user_name': 'Server', 'message': '<b><i>debug enabled, starting game.</i></b>'}, room=self.roomID)
			else:
				return
		if set(self.voteToStart) == set(self.activeRoomUsers.keys()):
			print ('ALL USERS READY TO START yeet')
			self.activeGame = True
			socketio.emit('startGame', room=self.roomID)
			self.allactiveRoomUsers.update(self.activeRoomUsers)
			self.cardList = {}
			self.ocelotManager = OcelotManager.gameBackend(list(self.allactiveRoomUsers.keys()))

			cards = self.ocelotManager.listCards()
			for i in cards:
				print (cards[i])
				self.cardList[cards[i]['id']] = {'name': i , 'params': cards[i]['id']}


			print (self.cardList)
			self.dumpAndSend()




	def dumpAndSend(self):
		ocelotDump = (self.ocelotManager.dumpState())
		for player in ocelotDump['players']:
			hand = ""
			print((ocelotDump['players'][player]['hand']))
			for i in (ocelotDump['players'][player]['hand']):
					hand = hand + self.cardList[i]['name'] + "! "
			socketio.emit('MessageResponse', {'user_name': 'Server', 'message': ('Your hand is ' + hand )}, room=self.allactiveRoomUsers[str(player)])




#####
GetRid = game_room('lukshans room')
#####
def reloadSocketPages():
	time.sleep(3)
	socketio.emit('ReloadPage')

@app.route('/enablepoll')
def enablePoll():
	if debug:
		Thread(target = checkAliveCycle, args =(False,) ).start()
		return redirect(url_for('home'))
	else:
		print ('aborting')
		abort()


if __name__ == '__main__':
	debug = True
	Thread(target = checkAliveCycle, args =(debug,) ).start()
	Thread(target = reloadSocketPages ).start()
	app = socketio.run(app, debug=debug, host='0.0.0.0')

	#socketio.run(app, debug=True)
