#include <iostream>
#include "Connection.h"


int main() {


    Connection *connection = new Connection("127.0.0.1", 4415);
    connection->connection();


    return 0;
}
