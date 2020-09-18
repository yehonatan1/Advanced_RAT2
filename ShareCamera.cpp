//
// Created by avita on 09/08/2020.
//

#include "ShareScreen.h"

ShareScreen::ShareScreen() {}


//Taking screenshot
void ShareScreen::takeScreenShot() {
    using namespace Gdiplus;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    {
        HDC scrdc, memdc;
        HBITMAP membit;
        scrdc = ::GetDC(0);

        memdc = CreateCompatibleDC(scrdc);
        membit = CreateCompatibleBitmap(scrdc, Width, Height);
        HBITMAP hOldBitmap = (HBITMAP) SelectObject(memdc, membit);
        BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
        Gdiplus::Bitmap bitmap(membit, NULL);
        CLSID clsid;
        GetEncoderClsid(L"image/jpeg", &clsid);
        //        bitmap.Save((IStream *) path, &clsid);
        //bitmap.Save(path, &clsid);
        if (!change) {
            insertColor(bitmap, img1);
            change = !change;
        } else {
            insertColor(bitmap, img2);
            change = !change;
        }

        SelectObject(memdc, hOldBitmap);
        DeleteObject(memdc);
        DeleteObject(membit);
        ::ReleaseDC(0, scrdc);
    }
    GdiplusShutdown(gdiplusToken);
}

//Help function to takeScreenShot
int ShareScreen::GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
    UINT num = 0; // number of image encoders
    UINT size = 0; // size of the image encoder array in bytes
    ImageCodecInfo *pImageCodecInfo = NULL;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1; // Failure
    pImageCodecInfo = (ImageCodecInfo *) (malloc(size));
    if (pImageCodecInfo == NULL)
        return -1; // Failure
    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j; // Success
        }
    }
    free(pImageCodecInfo);
    return 0;
}

//Insert to any pixel in pixels a color from img1 or img2
void ShareScreen::insertColor(Bitmap &bitmap, Color **pixels[1440][2580]) {
    Color color;
    for (int i = 0; i < 1440; i++) {
        for (int j = 0; j < 2580; j++) {
            bitmap.GetPixel(i, j, &color);
            **pixels[i][j] = color;
        }
    }
}

//Comparing two images and returning the bytes that changed
Color **ShareScreen::compareImages(Color **img1[1440][2580], Color **img2[1440][2580]) {


    Color **imgDifference = nullptr;
    imgDifference = reinterpret_cast<Color **>(new int *[1440]);
    for (int h = 0; h < 1440; h++) {
        imgDifference[h] = reinterpret_cast<Color *>(new int[2580]);
        for (int w = 0; w < 2580; w++) {
            if ((**img1[h][w]).GetValue() != (**img2[h][w]).GetValue()) {
                if (change) {
                    imgDifference[h][w] = **img2[h][w];
                    continue;
                }
                imgDifference[h][w] = **img1[h][w];
                continue;
            }
            imgDifference[h][w] = NULL;
        }
    }
    return imgDifference;
}

//Screen sharing in the first time sending img1 and after sending the bytes that changed between img1 and img2
void ShareScreen::ShareScreenLive(SOCKET socket) {
    //This will be sending to the server if img1 != img2
    Color **changedBytes = nullptr;


    //First sending the first image (img1)
    takeScreenShot();
    send(socket, reinterpret_cast<const char *>(img1), sizeof(img1), 0);
    //Sending every time the bytes that changed
    try{
        while (true) {
            takeScreenShot();
            changedBytes = compareImages(img1, img2);
            send(socket, reinterpret_cast<const char *>(changedBytes), sizeof(changedBytes), 0);

        }

    } catch (exception &e) {
        cout << e.what() << endl;
    }
}


