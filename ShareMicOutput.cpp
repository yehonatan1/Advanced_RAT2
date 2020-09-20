//
// Created by avita on 15/09/2020.
//

#include "ShareMicOutput.h"

ShareMicOutput::ShareMicOutput() {};

void ShareMicOutput::streamMic(SOCKET socket) {

    //Checking if the mic is available
    if (!sf::SoundBufferRecorder::isAvailable()) {
        connection->sendMessage("No mic was found exit!");
        return;
    }

    while (true) {
        recorder.start();
        Sleep(1000);
        recorder.stop();
        *buffer = recorder.getBuffer();
        connection->sendMessage(reinterpret_cast<const char *>(buffer));
    }
}
