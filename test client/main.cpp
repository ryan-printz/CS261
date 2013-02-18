#include "Engine.h"
#include "Socket.h"
#include "InputThreading.h"

#include <iostream>

int main(void)
{
    if( !initSockets(true) )
        return 1;

    Engine Test;

    StartInputThread();

    HSession testSession = Test.ConnectTcp("192.168.1.101", 4001);

    std::string buffer;

    while (1) {
        Test.Update(0);

        bool newInput = CheckInput(buffer);

        if (newInput) {
            Test.Send(buffer, testSession);
            buffer.clear();
        }
        buffer.resize(256);

        int result = Test.Receive((unsigned char *)&buffer.front(), 256);

        if (result != 0) {
            printf(buffer.c_str());
        }

        buffer.clear();
    }

    // cleanup.
    cleanSockets();
    StopInputThread();

    return 0;
}