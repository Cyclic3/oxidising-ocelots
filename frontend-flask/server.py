from flask import Flask, abort, render_template, redirect, make_response, url_for, flash
from flask import request

import re
rooms = []

app = Flask(__name__)

app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'


@app.route("/", methods = ["GET","POST"])
def home():
	print(listRooms())
	userData = (request.cookies.get('username'), sanitiseInput(request.cookies.get('userID')), listRooms())

	if request.method == 'GET':
		if not userData[0] and not userData[1]:
			return render_template("Home.html", userData = userData, askUsername = True, sidebar = True)
		else:
			return render_template("Home.html", userData = userData, askUsername = False, sidebar = True)


	if request.method == 'POST':
		if not userData[0] and not userData[1]:
			resp = make_response(redirect(url_for('home')))
			resp.set_cookie('username', sanitiseInput(request.form['username']))
			resp.set_cookie('userID', generateID(0xFFFFFFFF))
			return resp
		else:
			pass#Do room stuff



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

	if not request.remote_addr.startswith('127'):
		abort(403)

	if request.method == 'GET':
		return (render_template("CreateRoom.html", userData = userData))
	elif request.method == 'POST':
		game_room(request.form['roomName'])
		flash("Room has been created", 'success')
		return redirect(url_for('home'))


def sanitiseInput(input):
	if input:
		output = re.sub(r'[^\w]', '', input)
		return output
	else:
		return None



def generateID(val):
	import random
	return ("{:06x}".format(random.randint(0, val)))


def listRooms():
	a = []
	for i in rooms:
		a.append([i.roomName, i.roomID, len(i.roomUsers), i.activeGame])
	return a


class game_room:
	def __init__(self, roomName):
		self.roomName = roomName
		self.roomID = generateID(0xFFFFFF)
		self.roomUsers = []
		self.activeGame = False
		self.AddToActiveRooms()

	def AddToActiveRooms(self):
		rooms.append(self)

	def RemoveFromActiveRooms(self):
		rooms.remove(self)
