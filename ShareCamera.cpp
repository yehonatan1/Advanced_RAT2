//
// Created by avita on 15/09/2020.
//

#include "ShareCamera.h"

ShareCamera::ShareCamera(SOCKET soc) {
    sock = soc;

//    //Converting the socket to udp
//    sock = socket(AF_INET, SOCK_DGRAM, 0);
}

void ShareCamera::ShareCameraLive() {

    //Sending frame1 for the first time
    VideoCapture camera(0);
    vector<uchar> imgToSend = {};
    if (!camera.isOpened()) {       //If there is no a camera or cant open the camera

        //send(sock, "Can't open the camera", sizeof("Can't open the camera"), 0);
        send(sock, "$$$$$", 5, 0);

        return;
    }
    while (true) {
        try {
            camera >> frame1;

            imencode(".jpg", frame1, imgToSend);
            string imgSize = to_string(imgToSend.size() * sizeof(uchar));

            //Sending the size of imgToSend.data()
            send(sock, imgSize.c_str(), imgSize.size(), 0);


            cout << imgToSend.size() * sizeof(uchar) << endl;

            //Sending imgToSend.data()
            send(sock, reinterpret_cast<const char *>(imgToSend.data()), imgToSend.size() * sizeof(uchar), 0);
        } catch (exception &e) {
            cout << e.what() << endl;
//            if (camera.isOpened()) {
//                send(sock, "The camera was disconnected", 27, 0);
//                break;
//            }

            //Got exception on the client
            //send(sock, "Got e", 5, 0);
            send(sock, "#####", 5, 0);
            return;
        }
    }
}