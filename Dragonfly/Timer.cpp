#include "stdafx.h"
#include "Timer.h"


Timer::Timer() : m_SecondsPerCount(0.0), m_DeltaTime(-1.0), m_BaseTime({ 0 }),
m_PausedTime({ 0 }), m_StopTime({ 0 }), m_PrevTime({ 0 }), m_CurrentTime({ 0 }), m_Stopped(false)
{
	LARGE_INTEGER countsPerSec;
	QueryPerformanceFrequency(&countsPerSec);
	m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec.QuadPart);
}


Timer::~Timer()
{
}

void Timer::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}
	//Get the time this frame
	LARGE_INTEGER currTime;
	QueryPerformanceCounter(&currTime);
	m_CurrentTime = currTime;

	//Time difference between this frame and the previous
	m_DeltaTime = (m_CurrentTime.QuadPart - m_PrevTime.QuadPart)*m_SecondsPerCount;

	//Prepare for next frame
	m_PrevTime = m_CurrentTime;

	if (m_DeltaTime < 0.0)
		m_DeltaTime = 0.0;
}

float Timer::DeltaTime() const
{
	return static_cast<float>(m_DeltaTime);
}

void Timer::Reset()
{
	LARGE_INTEGER currTime;
	QueryPerformanceCounter(&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = { 0 };
	m_Stopped = false;
}

void Timer::Start()
{
	LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);

	// Accumulate the time elapsed between stop and start pairs.
	//
	// |<-------d------->|
	// ---------------*-----------------*------------> time
	// mStopTime startTime
	// If we are resuming the timer from a stopped state...
	if (m_Stopped)
	{
		// then accumulate the paused time.
		m_PausedTime.QuadPart += (startTime.QuadPart - m_StopTime.QuadPart);
		// since we are starting the timer back up, the current
		// previous time is not valid, as it occurred while paused.
		// So reset it to the current time.
		m_PrevTime = startTime;
		// no longer stopped...
		m_StopTime = { 0 };
		m_Stopped = false;
	}
}

void Timer::Stop()
{
	// If we are already stopped, then don't do anything.
	if (!m_Stopped)
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		// Otherwise, save the time we stopped at, and set
		// the Boolean flag indicating the timer is stopped.
		m_StopTime = currTime;
		m_Stopped = true;
	}
}

float Timer::TotalTime() const
{
	// If we are stopped, do not count the time that has passed
	// since we stopped. Moreover, if we previously already had
	// a pause, the distance mStopTime - mBaseTime includes paused
	// time,which we do not want to count. To correct this, we can
	// subtract the paused time from mStopTime:
	//
	// previous paused time
	// |<----------->|
	// ---*------------*-------------*-------*-----------*------> time
	// mBaseTime mStopTime mCurrTime
	if (m_Stopped)
	{
		return static_cast<float>((((m_StopTime.QuadPart - m_PausedTime.QuadPart) -
			m_BaseTime.QuadPart))*m_SecondsPerCount);
	}
	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count. To correct this, we can subtract
	// the paused time from mCurrTime:
	//
	// (mCurrTime - mPausedTime) - mBaseTime
	//
	// |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	// mBaseTime mStopTime startTime mCurrTime
	else
	{
		return static_cast<float>((((m_CurrentTime.QuadPart - m_PausedTime.QuadPart) -
			m_BaseTime.QuadPart)*m_SecondsPerCount));
	}
}