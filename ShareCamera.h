//
// Created by avita on 15/09/2020.
//

#ifndef ADVANCED_RAT2_SHARECAMERA_H
#define ADVANCED_RAT2_SHARECAMERA_H


#include <iostream>
#include <opencv2/opencv.hpp>
#include <WS2tcpip.h>

#pragma warning(disable:4996)
#pragma comment (lib, "Ws2_32.lib")

using namespace std;
using namespace cv;


class ShareCamera {


public:
    //Constructor
    ShareCamera(SOCKET soc);


    //Comparing two frames and send the diffrences
    void ShareCameraLive();

    //SOCKET of the class
    SOCKET sock;

    //Frames
    Mat frame1;

};


#endif //ADVANCED_RAT2_SHARECAMERA_H
