#include "Engine.h"
#include "Socket.h"
#include "InputThreading.h"
#include "Timer.h"

#include <iostream>

bool InitServerTest (Engine & engine) {

    if(!initSockets(true))
        return false;

    if (!engine.ToggleListenTcp(4001))
        return false;

    if (!StartInputThread())
        return false;

    return true;
}


int main(void)
{
	Engine testEngine;
    InitServerTest(testEngine);

    Timer timer;

    std::string buffer;

    while (1) {
        double dt = timer.Update();

        bool newInput = CheckInput(buffer);
        
        if (newInput) {
            // send call should go here
            buffer.clear();
        }
        buffer.resize(256);

        int result = testEngine.Receive((unsigned char *)&buffer.front(), 256);

        if (result != 0) {
            printf(buffer.c_str());
        }


    }

	// cleanup.
	cleanSockets();
    StopInputThread();

	return 0;
}