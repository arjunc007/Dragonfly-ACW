#pragma once
class Timer
{
private:
	double m_SecondsPerCount;
	double m_DeltaTime;

	LARGE_INTEGER m_BaseTime;
	LARGE_INTEGER m_PausedTime;
	LARGE_INTEGER m_StopTime;
	LARGE_INTEGER m_PrevTime;
	LARGE_INTEGER m_CurrentTime;

	bool m_Stopped;

public:
	Timer();
	~Timer();

	float TotalTime() const;	//in seconds
	float DeltaTime() const;	//in seconds

	void Reset();				//Call before message loop
	void Start();				//Call when unpaused
	void Stop();				//Call when paused
	void Tick();				//Call every frame
};

