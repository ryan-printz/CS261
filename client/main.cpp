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

    client.Initialize();

    bool quit = false;

    while (!quit) {
        quit = client.Update();
    }

    client.ShutDown();

    return 0;
}