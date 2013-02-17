/*
*       InputThreading.cpp
*
*       by Tim Rowsey
*
*       2013/02/16
*/

#include <Windows.h>

static std::string      s_messageStream;
static CRITICAL_SECTION s_critSect;
static HANDLE           s_threadHandle;
static HANDLE           s_termEvent;
static bool             s_isActive = false;

DWORD WINAPI ThreadInputFunction( LPVOID param ) {
    std::string buffer;

    while (1) {
        DWORD result = WaitForSingleObject( s_termEvent, 0 );

        switch ( result ) {

        case WAIT_OBJECT_0:
            {
                std::cout << "Closing client input thread. " << std::endl;
                return 0;
            }

        case WAIT_TIMEOUT: 
            {
                getline( std::cin, buffer );

                bool written = false;

                while ( !written ) {
                    EnterCriticalSection( s_critSect );
                    if ( s_messageStream != 0 ) {
                        LeaveCriticalSection( s_critSect );
                        Sleep(1);
                        continue;
                    }

                    s_messageStream = buffer;
                    written = true;
                    LeaveCriticalSection(s_critSect);
                }
                break;
            }
        default:
            assert( false );
        }
    }
}

bool CheckInput       (std::string & buffer) {
    if (!s_isActive) {
        StartInputThread ();
    }

    bool success = false;

    EnterCriticalSection(s_critSect);

    if (s_messageStream != 0) {
        buffer = s_messageStream;
        s_messageStream.clear();
        success = true;
    }

    return success;
}

bool StartInputThread () {
    if (s_isActive)
        return true;

    s_termEvent = CreateEvent(NULL, true, false, NULL);
    InitializeCriticalSection(s_critSect);
    s_threadHandle = CreateThread(
        NULL,
        ThreadInputFunction,
        NULL,
        0,
        NULL
    );

    s_isActive = true;
}

bool StopInputThread  () {
    if (!s_isActive)
        return true;

    SetEvent( m_TerminationEvent );
    CloseHandle( m_ThreadHandle );
    DeleteCriticalSection( &m_CriticalSection );

    s_isActive = false;
}