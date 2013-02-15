#include "Engine.h"
#include "Socket.h"

#include <iostream>

int main(void)
{
    if( !initSockets(true) )
        return 1;

    Engine Test;

    HSession testSession = Test.ConnectTcp("192.168.1.101", 4001);

    std::string testMessage("Message Fucking Received.");
    Test.Send(testMessage, testSession);

    while (1) {
        Test.Update(0);
    }

    // cleanup.
    cleanSockets();
    return 0;
}