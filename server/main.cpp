/*
*       ServerMain.cpp
*
*       by Tim Rowsey
*
*       2013/02/18
*/
#include "FileShareServer.h"

int main (void) {
    FileShareServer server;

    server.Initialize();

    bool quit = false;

    while (!quit) {
        quit = server.Update();
    }

    server.ShutDown();

    return 0;
}