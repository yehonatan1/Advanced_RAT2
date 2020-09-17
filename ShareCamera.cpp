//
// Created by avita on 15/09/2020.
//

#include "ShareCamera.h"


ShareCamera::ShareCamera() {};

Mat *compareFrames(Mat *frame1, Mat *frame2) {

    vector<int> *changedBytes = {};

    for (int x = 0; x < frame1->cols; x++) {
        for (int y = 0; y < frame1->rows; y++) {
            size_t index = y * frame1->cols * 3 + x * 3;
            if (frame1->data[index + 0] != frame2->data[index + 0] ||
                frame1->data[index + 1] != frame2->data[index + 1] ||
                frame1->data[index + 2] != frame2->data[index + 2]) {

                changedBytes->push_back(frame2->data[index + 0]);
                changedBytes->push_back(frame2->data[index + 1]);
                changedBytes->push_back(frame2->data[index + 2]);
                continue;
                /*
                frame1->data[index + 0] = frame2->data[index + 0];
                frame1->data[index + 1] = frame2->data[index + 1];
                frame1->data[index + 2] = frame2->data[index + 2];
                */

            }
            frame1->data[index + 0] = NULL;
            frame1->data[index + 1] = NULL;
            frame1->data[index + 2] = NULL;
        }
    }
    return frame1;
}


void ShareCamera::ShareCameraLive(SOCKET socket) {

    //Sending frame1 for the first time
    VideoCapture camera(0);
    camera >> frame1;
    send(socket, reinterpret_cast<const char *>(frame1.data), frame1.total() * frame1.elemSize(), 0);

    Mat *changedBytes = nullptr;


    //Comparing and sending frame1 and frame2
    try {
        while (true) {

            if (compare) {
                camera >> frame2;
                changedBytes = compareFrames(&frame2, &frame1);
                send(socket, reinterpret_cast<const char *>(changedBytes->data),
                     changedBytes->total() * changedBytes->elemSize(), 0);
            } else {
                camera >> frame1;
                changedBytes = compareFrames(&frame1, &frame2);
                send(socket, reinterpret_cast<const char *>(changedBytes->data),
                     changedBytes->total() * changedBytes->elemSize(), 0);
            }
        }
    }
    catch (exception &e) {
        cout << e.what() << endl;
    }
}