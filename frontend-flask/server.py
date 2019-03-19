try:
	from flask import Flask, abort, render_template, redirect, make_response, url_for, flash, request
	from flask_socketio import SocketIO

except:
	while True:
		input("You are missing dependencies. Please use pip to install the following \n Flask \n Flask-SocketIO==3.1.2")
import re
rooms = {}


app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'
socketio = SocketIO(app)


@app.route("/", methods = ["GET","POST"])
def home():
	print(listRooms())
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())

	if (request.remote_addr.startswith('127') or request.remote_addr == '::1'):
		printRef = True
	else:
		printRef = False

	if request.method == 'GET':
		if not userData[0] and not userData[1]:
			return render_template("Home.html", userData = userData, askUsername = True, sidebar = True, printRef = printRef)
		else:
			return render_template("Home.html", userData = userData, askUsername = False, sidebar = True, printRef = printRef)


	if request.method == 'POST':

		if request.form['mode'] == 'userjoin':
			print (request.form)
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
				pass



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
	if not roomRefrence in rooms:
		flash('That room cannot be found','warning')
		return redirect(url_for('home'))
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())
	return render_template('room.html', userData = userData)


def messageReceived(methods=['GET', 'POST']):
    print('message was received!!!')


@socketio.on('chat')
def handle_my_custom_event(json, methods=['GET', 'POST']):
    print('received my event: ' + str(json))
    socketio.emit('my response', json, callback=messageReceived)




#<-------------------------------------------------------------------------------->

def sanitiseInput(input):
	if input:
		output = re.sub(r'[^\w\s]', '', input)
		return output
	else:
		return None



def generateID(val):
	import random
	return ("{:06x}".format(random.randint(0, val)))


def listRooms():
	a = []
	print (rooms)
	for i in rooms:
		i = (rooms[i])
		a.append([i.roomName, i.roomID, len(i.roomUsers), i.activeGame])
	return a


class game_room:
	def __init__(self, roomName):
		self.roomName = roomName
		self.roomID = generateID(0xFFFFFFFF)
		self.roomUsers = []
		self.activeGame = False
		self.AddToActiveRooms()

	def AddToActiveRooms(self):
		rooms.update({self.roomID: self})

	def RemoveFromActiveRooms(self):
		del rooms[self.roomID]

	def AddUserToRoom(self, userID):
		self.roomUsers.append(userID)

if __name__ == '__main__':
    socketio.run(app, debug=True, host='0.0.0.0')
