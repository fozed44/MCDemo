#pragma once

namespace MC {
	class MasterTimer {
	public:
		MasterTimer();
		~MasterTimer();

		MasterTimer(const MasterTimer&) = delete;
		MasterTimer(const MasterTimer&&) = delete;
		const MasterTimer& operator=(const MasterTimer&) = delete;
	public:
		void  Reset();
		void  Tick();
		float TotalTime();
		float DeltaTime() const;
		void  Pause();
		void  Unpause();
	private:
		double _secondsPerCount;
		double _deltaTime;

		__int64 _baseTime;
		__int64 _pausedTime;
		__int64 _stopTime;

		__int64 _previousTime;
		__int64 _currentTime;		
		
		bool    _paused;
	};
}
