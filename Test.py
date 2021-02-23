# import datetime
# import time
# import webbrowser
# import os
#
# target_time = datetime.datetime(2021, 2, 12, 11, 50)
#
# while datetime.datetime.now() < target_time:
#     time.sleep(10)
# print('It is 8:45 am, now running the rest of the code')
# url = "https://us04web.zoom.us/j/74092507701?pwd=TlcxYmJOUjU0aDc1TmxVQUJ6N3FDZz09"
# webbrowser.get("C:/Program Files (x86)/Google/Chrome/Application/chrome.exe %s").open(url)
#
# target_time = datetime.datetime(2021, 2, 12, 11, 51)
# while datetime.datetime.now() < target_time:
#     time.sleep(10)
#
# os.system("TASKKILL /F /IM zoom.exe")


import socket
import threading
from datetime import datetime

PORT = 5050
HOST = socket.gethostbyname(socket.gethostname())

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen()

clients = []
nicknames = []


def broadcast(msg):
    # print('message is ' + str(msg))
    # print(str(msg).encode())
    for client in clients:
        client.send(msg)
        client.send(str(msg).encode())


def handle(client):
    while True:
        try:
            msg = client.recv(1024).decode('utf-8')
            print(msg)
            msg1 = msg.split(":")
            if msg1[3] == ' quit':
                print('+' * 10)
            broadcast(msg)  # bug
        except:
            index = clients.index(client)
            clients.remove(client)
            client.close()
            nickname = nicknames[index]
            broadcast(f"{datetime.now()} {nickname} has left the chat :(".encode())
            nicknames.remove(nickname)
            break


def receive():
    while True:
        client, address = server.accept()
        print(f"Connected with {str(address)}")

        client.send('NICK'.encode())
        nickname = client.recv(1024).decode()
        nicknames.append(nickname)
        clients.append(client)

        print(f"Nickname of the client is {nickname}!")

        broadcast(f'{nickname} has joined the chat :)'.encode())
        client.send('You are now connected to the chat!'.encode())

        thread = threading.Thread(target=handle, args=(client,))
        thread.start()


print("Server is listening...")
receive()
