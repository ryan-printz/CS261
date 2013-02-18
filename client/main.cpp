/*
*       main.cpp
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#include "FileShareClient.h"

int main (void) {
    FileShareClient client;

    server.Initialize();

    bool quit = false;

    while (!quit) {
        quit = server.Update();
    }

    server.ShutDown();

    return 0;
}