//
// Created by avita on 15/09/2020.
//

#include "ShareMicOutput.h"

ShareMicOutput::ShareMicOutput(SOCKET sock) {
    socket = sock;
}

void ShareMicOutput::streamMic() {

    //Checking if the mic is available
    if (!sf::SoundBufferRecorder::isAvailable()) {
        send(socket, "No mic was found exit!", 22, 0);
        return;
    }

    //string samplesCount;


    while (true) {
        recorder.start(44100);
        Sleep(2000);
        recorder.stop();


        const sf::SoundBuffer &buffer = recorder.getBuffer();

        cout << "Samples " << buffer.getSamples() << endl;

        buffer.saveToFile("C:\\C projects\\audio.ogg");

        const sf::Int16 *samples = buffer.getSamples();
        const char *bytesData = reinterpret_cast<const char *>(samples);


//        cout << "Sample Count is " << buffer.getSampleCount() << endl;
//        cout << "const char* Sample Count is " << to_string(buffer.getSampleCount()).c_str() << endl;
        cout << "samples is " << samples << endl;
        send(socket, to_string(buffer.getSampleCount()).c_str(), to_string(buffer.getSampleCount()).size(), 0);


        //Sending array of samples of the audio

        cout << "Bytes data size is " << strlen(bytesData) << endl;
        cout << "Bytes data is " << bytesData << endl;

        send(socket, bytesData, buffer.getSampleCount(), 0);
        return;
    }
}
