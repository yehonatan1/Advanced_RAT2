#include <iostream>
#include "Connection.h"


int main() {


    string ip = "192.168.1.210";
    int port = 9999;


    Connection *connection = new Connection(ip, port);
    connection->connection();


    return 0;
}
