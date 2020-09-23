//
// Created by avita on 15/09/2020.
//

#ifndef ADVANCED_RAT2_SHARECAMERA_H
#define ADVANCED_RAT2_SHARECAMERA_H


#include <iostream>
#include <opencv2/opencv.hpp>
#include <WS2tcpip.h>
#include <vector>

#pragma warning(disable:4996)
#pragma comment (lib, "Ws2_32.lib")

using namespace std;
using namespace cv;


class ShareCamera {


public:
    //Constructor
    ShareCamera(SOCKET sock);


    //Compare two Mat frames
    vector<int32_t> compareFrames(Mat *frame_1, Mat *frame_2);


    //Comparing two frames and send the diffrences
    void ShareCameraLive();

    //SOCKET of the class
    SOCKET socket;

    //Frames
    Mat frame1;
    Mat frame2;

    //Tell who frames to compare
    bool compare = true;
};


#endif //ADVANCED_RAT2_SHARECAMERA_H
