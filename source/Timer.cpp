/*
*       Timer.cpp
*
*       by Tim Rowsey
*
*       2013/02/17
*/

#include "Timer.h"
#include <Windows.h>

#pragma comment(lib,"Winmm.lib")

//******************************************************************************
Timer::Timer () {
    DWORD milliSec = timeGetTime();

    m_startTime = m_lastTime = double(milliSec)/1000.0;
}

//******************************************************************************
Timer::~Timer () {

}

//******************************************************************************
double Timer::Update () {

    double currentTime = double(timeGetTime())/1000.0;

    m_frameTime = currentTime - m_lastTime;

    m_lastTime = currentTime;

    return m_frameTime;
}

//******************************************************************************
double Timer::GetFrameTime () const {
    return m_frameTime;
}

//******************************************************************************
double Timer::GetRunTime () const {
    return m_lastTime - m_startTime;
}