try:
	from flask import Flask, abort, render_template, redirect, make_response, url_for, flash, request
	from flask_socketio import SocketIO

except:
	while True:
		input("You are missing dependencies. Please use pip to install the following \n Flask \n Flask-SocketIO==3.1.2")
import re, json, time
from threading import Thread
rooms = {}


oldList = activeList = []
USER_TIMEOUT = 10




def checkAlive(sleeper):
	time.sleep(sleeper)
	global activeList, oldList
	oldList = activeList
	activeList = []
	socketio.emit('AreYouAlive')

	print ("Poll sent")
		
	time.sleep(sleeper)		

	if len(oldList) == 0:
		joinedUsers = activeList
		leftUsers = []
	elif len(activeList)  == 0:
		leftUsers = oldList
		joinedUsers = []

	else:
		joinedUsers = list(set(oldList)-set(activeList))
		leftUsers = list(set(oldList)-set(activeList))

	if not leftUsers == []:
		for room in rooms:
			for user in oldList:
				if user in rooms[room].activeRoomUsers:
					rooms[room].RemoveActiveUserFromRoom(user)

	return('New Users:' +str(joinedUsers)+ '.  users who have left:' +str(leftUsers))


def checkAliveCycle(debug):
	if debug:
		print('auto polling not active while in debug mode. Please run server with debug = False')
		return
	while True:
		print (checkAlive(USER_TIMEOUT))



app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 300
socketio = SocketIO(app)




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

	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())


	for room in rooms:
		print (rooms[room].activeRoomUsers)
		if userData[1] in rooms[room].activeRoomUsers:
			flash("You appear to already be in a room. Perhaps you are in another tab? If you have just left a game, please wait upto 10 seconds before trying to connect", 'warning')
			return redirect(url_for('home'))
	return render_template('room.html', userData = userData)



@socketio.on('IAmAlive')
def amAliveNotification():
	global activeList
	activeList.append(request.cookies.get('userID'))


def messageReceived(methods=['GET', 'POST']):
    print('message was received!!!')


@socketio.on('connect')
def on_connect():
	activeList.append(request.cookies.get('userID'))
	jsonData = {
	'user_name': sanitiseInput(request.cookies.get('username')),
	'message': '<b><i>User Connected</i></b>'
	}
	socketio.emit('ActionResponse', jsonData, callback=messageReceived)

@socketio.on('disconnect')
def on_disconnect():

    jsonData = {
    'user_name': sanitiseInput(request.cookies.get('username')),
    'message': '<b><i>User Disconnected</i></b>'
    }

    print ('\n\nuserdisconnected\n\n')
    socketio.emit('ActionResponse', jsonData, callback=messageReceived)


@socketio.on('registerToRoom')
def regsiterToRoom(data, methods=['GET', 'POST']):
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())

	lis = (request.sid)


	roomRefrence = (data['roomID'])
	currentRoom = rooms.get(roomRefrence)
	currentRoom.activeRoomUsers[userData[1]] = (lis)


@socketio.on('ActionHappened')
def handle_my_custom_event(jsonData, methods=['GET', 'POST']):

	print (jsonData)
	jsonData['username'] = request.cookies.get('username')
	jsonData['user_name'] = request.cookies.get('username')

	for i in jsonData:
		jsonData[i] = sanitiseInput(jsonData[i])

	if 'messageBold' in jsonData:
		jsonData['message'] = ("<b><i>"+jsonData.pop('messageBold')+"</i></b>")


	socketio.emit('ActionResponse', jsonData, callback=messageReceived)




########################################################################################################################

def sanitiseInput(input):
	if input:
		output = re.sub(r'[^\w\s]', '', input)
		return output
	else:
		return input



def generateID(val):
	import random
	return ("{:06x}".format(random.randint(0, val)))


def listRooms():
	a = []
	for i in rooms:
		i = (rooms[i])
		a.append([i.roomName, i.roomID, len(i.activeRoomUsers), i.activeGame])
	return a


class game_room:
	def __init__(self, roomName):
		self.roomName = roomName
		self.roomID = generateID(0xFFFFFFFF)
		self.activeRoomUsers = {}
		self.allactiveRoomUsers = {}
		self.activeGame = False
		self.AddToActiveRooms()

	def AddToActiveRooms(self):
		rooms.update({self.roomID: self})

	def RemoveFromActiveRooms(self):
		del rooms[self.roomID]

	def AddUserToRoom(self, userID):
		self.activeRoomUsers.append(userID)

	def RemoveActiveUserFromRoom(self, userID):
		del self.activeRoomUsers[userID]


#####
GetRid = game_room('lukshans room')
#####


if __name__ == '__main__':
	debug = False
	Thread(target = checkAliveCycle, args =(debug,) ).start()
	app = socketio.run(app, debug=debug, host='0.0.0.0')
	#socketio.run(app, debug=True)
