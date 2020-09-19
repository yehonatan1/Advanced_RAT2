//
// Created by avita on 15/09/2020.
//

#include "ShareCamera.h"


ShareCamera::ShareCamera() {};


//Returning int32_t vector
vector<int32_t> *compareFrames(Mat *frame1, Mat *frame2) {


    vector<int32_t> *changedBytes = {};

    for (int x = 0; x < frame1->cols; x++) {
        for (int y = 0; y < frame1->rows; y++) {
            size_t index = y * frame1->cols * 3 + x * 3;
            if (frame1->data[index + 0] != frame2->data[index + 0] ||
                frame1->data[index + 1] != frame2->data[index + 1] ||
                frame1->data[index + 2] != frame2->data[index + 2]) {

                //The first element is the index of the pixels the third fourth and fifth elements are B.G.R
                //htonl is a network byte int
                changedBytes->push_back(htonl(index)); //Pushing the index of the frame
                changedBytes->push_back(htonl(frame2->data[index + 0])); //B
                changedBytes->push_back(htonl(frame2->data[index + 1])); //G
                changedBytes->push_back(htonl(frame2->data[index + 2])); //R
                continue;
            }
        }
    }
    return changedBytes;
}

//Live sharing of the camera
void ShareCamera::ShareCameraLive(SOCKET socket) {

    //Sending frame1 for the first time
    VideoCapture camera(0);

    if (!camera.isOpened()) {       //If there is no a camera or cant open the camera
        send(socket, "Can't open the camera", sizeof("Can't open the camera"), 0);
    }

    camera >> frame1;
    send(socket, reinterpret_cast<const char *>(frame1.data), frame1.total() * frame1.elemSize(), 0);

    vector<int32_t> *changedBytes = {};

    try {  //Comparing and sending frame1 and frame2
        while (true) {

            if (compare) {             //If compare is true so compareFrames will compare frame2 to frame1
                camera >> frame2;
                changedBytes = compareFrames(&frame2, &frame1);
                send(socket, reinterpret_cast<const char *>(changedBytes), changedBytes->size(), 0);
            } else {                 //If compare is false so compareFrames will compare frame1 to frame2
                camera >> frame1;
                changedBytes = compareFrames(&frame1, &frame2);
                send(socket, reinterpret_cast<const char *>(changedBytes), changedBytes->size(), 0);
            }
        }
    }
    catch (exception &e) {          //Catching any exception
        cout << e.what() << endl;
    }
}