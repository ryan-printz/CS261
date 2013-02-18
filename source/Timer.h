/*
*       Timer.h
*
*       by Tim Rowsey
*
*       2013/02/17
*/

#pragma once

class Timer {
public:
    Timer  ();
    ~Timer ();

    // Only call once per frame!
    double Update  ();

    // If you need to access for whatever reason after update
    double GetFrameTime () const;
    double GetRunTime   () const;

private:
    double m_startTime;
    double m_lastTime;
    double m_frameTime;
    
};