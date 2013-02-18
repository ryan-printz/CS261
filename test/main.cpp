#include "Engine.h"
#include "Socket.h"
#include "InputThreading.h"
#include "Timer.h"
#include "Serialization.h"

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
	/*
	Packer p;
	int testArray[4] = {1, 2, 3, 4};

	TestEvent2 test2, unpackHere2;
	TestEvent1 test, unpackHere;
	test.test1 = 4;
	test.test2 = 7.5f;
	char buf[12];
	char buf2[20];
	unpackHere.test1 = 0;
	unpackHere.test2 = 0.f;
	unpackHere2.test1 = 0;
	unpackHere2.test2 = NULL;
	test2.test1 = 4;
	test2.test2 = testArray;

	p.pack(test, buf);
	p.pack(test2, buf2);

	printf("%d, %f\n", *(int*)buf, *((float*)(buf + 4)));

	printf("%d, %d, %d, %d, %d\n", ((int*)buf2)[0], ((int*)buf2)[1], ((int*)buf2)[2], ((int*)buf2)[3], ((int*)buf2)[4]);

	p.unpack(unpackHere, buf);

	printf("%d, %f\n", unpackHere.test1, unpackHere.test2);

	p.unpack(unpackHere2, buf2);

	printf("%d, %d, %d, %d, %d\n", unpackHere2.test1, unpackHere2.test2[0], unpackHere2.test2[1], unpackHere2.test2[2], unpackHere2.test2[3]);
	*/
    while (1) {

        bool newInput = CheckInput(buffer);
        
        testEngine.Update(timer.Update());

        if (newInput) {
            testEngine.Send(buffer, nullptr);
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