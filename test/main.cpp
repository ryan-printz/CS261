#include "Engine.h"
#include "Socket.h"

#include <iostream>

int main(void)
{
	if( !initSockets(true) )
		return 1;

	Engine Test;

    Test.ToggleListenTcp(4001);

    while (1) {
        Test.Update(0);

        unsigned char buffer[256];

        int result = Test.Receive(buffer, 256);

        if (result != 0) {
            printf((const char *)buffer);
            break;
        }
    }

	// cleanup.
	cleanSockets();
	return 0;
}