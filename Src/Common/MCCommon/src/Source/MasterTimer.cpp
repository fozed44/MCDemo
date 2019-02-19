#include "../Headers/MasterTimer.h"
#include "windows.h"
#include "../Headers/Utility.h"

namespace MC {

#pragma region Ctor

	MasterTimer::MasterTimer() {
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		_secondsPerCount = 1.0 / (double)countsPerSec;

		Reset();
	}

	MasterTimer::~MasterTimer() {}

#pragma endregion

#pragma region Public

	void MasterTimer::Reset() {
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		_baseTime     = currTime;
		_previousTime = currTime;
		_stopTime     = 0;
		_pausedTime   = 0;
		_paused       = false;
	}

	void MasterTimer::Tick() {
		if (_paused) {
			_deltaTime = 0.0;
			return;
		}

		// Get the time this frame.
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		_currentTime = currTime;

		// Calculate the time since last frame
		_deltaTime = (_currentTime - _previousTime)*_secondsPerCount;

		// Get ready for the next frame.
		_previousTime = _currentTime;

		// We have to Force _deltaTime to be non-negative. The DXSDK CDXUTTimer mentions that
		// if the processor goes into a power save mode or we get shuffled to another processor,
		// then _deltaTime can be negative.
		if (_deltaTime < 0.0)
			_deltaTime = 0.0;
	}

	float MasterTimer::DeltaTime() const {
		return (float)_deltaTime;
	}

	float MasterTimer::TotalTime() {
		if (_paused)
			return (float)(((_stopTime - _pausedTime) - _baseTime)*_secondsPerCount);
		else
			return (float)(((_currentTime - _pausedTime) - _baseTime)*_secondsPerCount);
	}

	void MasterTimer::Pause() {
		if (_paused)
			return;

		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		_stopTime = currTime;
		_paused   = true;
	}

	void MasterTimer::Unpause() {
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (_paused) {
			_pausedTime += startTime - _stopTime;

			_previousTime = startTime;

			_stopTime = 0;
			_paused = false;
		}
	}

#pragma endregion

}