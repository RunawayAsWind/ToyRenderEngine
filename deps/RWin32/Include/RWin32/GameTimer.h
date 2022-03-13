//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <unordered_map>
#include <ctime>
#include <windows.h>
#include "Win32Debug.h"

namespace RWin32
{
	class GameTimer
	{
	public:
		
		GameTimer();

		float TotalTime()const; // in seconds
		float DeltaTime()const; // in seconds

		void Reset(); // Call before message loop.
		void Start(); // Call when unpaused.
		void Stop();  // Call when paused.
		void Tick();  // Call every frame.
		void ShowInfo_Win32();
		void ShowInfo_Cout();
		
		std::unordered_map<std::string, float> costTimeDic;
		template<typename LmbdaFunc>
		void GetCostTime(std::string name,LmbdaFunc func)
		{
			clock_t start_time = clock();
			func();
			clock_t end_time = clock();
			costTimeDic[name] = end_time - start_time;
			//std::cout << name << " " << costTimeDic[name] << std::endl;
		}

	private:
		double mSecondsPerCount;
		double mDeltaTime;

		__int64 mBaseTime;
		__int64 mPausedTime;
		__int64 mStopTime;
		__int64 mPrevTime;
		__int64 mCurrTime;

		bool mStopped;
	};
}
#endif // GAMETIMER_H