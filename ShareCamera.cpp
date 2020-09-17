//
// Created by avita on 15/09/2020.
//

#include "ShareCamera.h"



ShareCamera::ShareCamera(){};

vector<unsigned int> *compareFrames(Mat* frame1, Mat* frame2) {



    vector<unsigned int>* changedBytes = {};

    for (int x = 0; x < frame1->cols; x++)
    {
        for (int y = 0; y < frame1->rows; y++)
        {
            size_t index = y * frame1->cols * 3 + x * 3;
            if (frame1->data[index + 0] != frame2->data[index + 0] || frame1->data[index + 1] != frame2->data[index + 1] || frame1->data[index + 2] != frame2->data[index + 2]) {


                //The first element is the index of the pixels the third fourth and fith elemets are B.G.R
                changedBytes->push_back(index);
                changedBytes->push_back(frame2->data[index + 0]); //B
                changedBytes->push_back(frame2->data[index + 1]); //G
                changedBytes->push_back(frame2->data[index + 2]); //R
                continue;
            }
        }
    }
    return changedBytes;
}


void ShareCamera::ShareCameraLive(SOCKET socket) {

    //Sending frame1 for the first time
    VideoCapture camera(0);
    camera >> frame1;
    send(socket, reinterpret_cast<const char*>(frame1.data), frame1.total() * frame1.elemSize(), 0);

    vector<unsigned int>* changedBytes = {};


    //Comparing and sending frame1 and frame2
    try {
        while (true) {

            if (compare) {
                camera >> frame2;
                changedBytes = compareFrames(&frame2, &frame1);
                send(socket, reinterpret_cast<const char*>(changedBytes), changedBytes->size(), 0);
            }
            else {
                camera >> frame1;
                changedBytes = compareFrames(&frame1, &frame2);
                send(socket, reinterpret_cast<const char*>(changedBytes), changedBytes->size(), 0);
            }
        }
    }
    catch (exception& e) {
        cout << e.what() << endl;
    }
}