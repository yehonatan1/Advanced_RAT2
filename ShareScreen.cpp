//
// Created by avita on 09/08/2020.
//

#include "ShareScreen.h"

ShareScreen::ShareScreen() {}

void ShareScreen::takeScreenShoot(const wchar_t *path) {
    using namespace Gdiplus;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    {
        HDC scrdc, memdc;
        HBITMAP membit;
        scrdc = ::GetDC(0);
        int Height = GetSystemMetrics(SM_CYSCREEN);
        int Width = GetSystemMetrics(SM_CXSCREEN);
        memdc = CreateCompatibleDC(scrdc);
        membit = CreateCompatibleBitmap(scrdc, Width, Height);
        HBITMAP hOldBitmap = (HBITMAP) SelectObject(memdc, membit);
        BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
        Gdiplus::Bitmap bitmap(membit, NULL);
        CLSID clsid;
        GetEncoderClsid(L"image/jpeg", &clsid);
        //        bitmap.Save((IStream *) path, &clsid);
        bitmap.Save(path, &clsid);
        SelectObject(memdc, hOldBitmap);
        DeleteObject(memdc);
        DeleteObject(membit);
        ::ReleaseDC(0, scrdc);
    }
    GdiplusShutdown(gdiplusToken);
}

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































