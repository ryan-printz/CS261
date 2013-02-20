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

class TimeInterp {
public:
    TimeInterp(float start, float end) 
        : m_start(start)
        , m_end(end)
        , m_accumulated(0) {}

    bool Update(float dt) {
        m_accumulated += dt;

        if (m_end - m_accumulated <= m_start) {
            m_accumulated = 0;
            return true;
        }

        return false;
    }

private:
    float m_start;
    float m_end;
    float m_accumulated;
};