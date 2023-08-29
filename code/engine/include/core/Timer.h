#ifndef Timer_h__
#define Timer_h__

namespace rush
{

	class Timer
	{
	public:
		static void Tick();

		static double GetTimeMicro();
		static double GetTimeMill();
		static double GetTimeSec();
		static double GetDeltaTimeMs();
		static double GetDeltaTimeSec();

		static int GetFPS();
		static float GetPeriodTime() { return m_FrameTimePeriod; }
		static float GetCPUTime() { return m_FrameCPUTime; }
		static float GetGPUTime() { return m_FrameGPUTime; }

        static void SleepSec(int time);
        static void SleepMill(int time);
		static void SleepMicro(int time);
		static void SleepNano(int time);

		static void Delay(double millSec, std::function<void()> callback);

        static void AddGpuTimerSample(float time);
        static void AddCpuTimerSample(float time);
        static void AddTotalTimerSample(float time);

	private:
		static double m_LastTickTime;
		static double m_DeltaTime;
		static int m_Fps;
		static int m_FrameNO;
		static double m_LastFpsTime;
        static float m_FrameGPUTime;
        static float m_FrameCPUTime;
        static float m_FrameTimePeriod;
		
        static float m_FrameGPUTime_C;
        static float m_FrameCPUTime_C;
        static float m_FrameTimePeriod_C;

		struct DelayCallback
		{
			double time = 0;
			double total = 0;
			std::function<void()> callback;
		};

		static std::list<DelayCallback> s_DelayCallbacks;
	};

}

#endif // Timer_h__
