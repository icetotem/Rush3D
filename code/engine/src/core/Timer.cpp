#include "stdafx.h"
#include "Core/Timer.h"

namespace rush
{

	double Timer::m_LastTickTime = 0;
	double Timer::m_DeltaTime = 0;
	int Timer::m_Fps = 1;
	int Timer::m_FrameNO = 1;
	double Timer::m_LastFpsTime = 0;

    float Timer::m_FrameGPUTime = 0;
    float Timer::m_FrameCPUTime = 0;
    float Timer::m_FrameTimePeriod = 0;
    float Timer::m_FrameGPUTime_C = 0;
    float Timer::m_FrameCPUTime_C = 0;
    float Timer::m_FrameTimePeriod_C = 0;

    float Timer::m_DetalMin = 0.001f;
    float Timer::m_DetalMax = 0.1f;

	std::list<Timer::DelayCallback> Timer::s_DelayCallbacks;

	void Timer::Tick()
	{
		double tick_time = GetTimeMill();
		if (m_LastTickTime <= 0)
			m_LastTickTime = tick_time;

		m_DeltaTime = tick_time - m_LastTickTime;
		m_LastTickTime = tick_time;

		if (tick_time - m_LastFpsTime >= 1000.0)
		{
			m_Fps = m_FrameNO;
			m_LastFpsTime = tick_time;

            m_FrameGPUTime = std::max(0.0f, m_FrameGPUTime_C / m_FrameNO);
            m_FrameCPUTime = std::max(0.0f, m_FrameCPUTime_C / m_FrameNO);
            m_FrameTimePeriod = std::max(0.0f, m_FrameTimePeriod_C / m_FrameNO);
			m_FrameGPUTime_C = 0;
			m_FrameCPUTime_C = 0;
			m_FrameTimePeriod_C = 0;
            m_FrameNO = 0;
        }

		++m_FrameNO;

		for (auto iter = s_DelayCallbacks.begin(); iter != s_DelayCallbacks.end();)
		{
			if (GetTimeMill() - (*iter).time > (*iter).total)
			{
				(*iter).callback();
				s_DelayCallbacks.erase(iter++);
			}
			else
				++iter;
		}
	}

    void Timer::SetDeltaTimeClamp(float _min, float _max)
    {

    }

    double Timer::GetTimeSec()
	{
		auto now = std::chrono::system_clock::now();
		uint64_t dis_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
		return (double)dis_seconds;
	}

	double Timer::GetTimeMicro()
	{
		auto now = std::chrono::system_clock::now();
		uint64_t dis_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		return (double)dis_microseconds;
	}

	double Timer::GetTimeMill()
	{
		auto now = std::chrono::system_clock::now();
		uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		return (double)dis_millseconds;
	}

	double Timer::GetDeltaTimeMs()
	{
		return glm::clamp<double>((double)m_DeltaTime, m_DetalMin * 1000.0, m_DetalMax * 1000.0);
	}

	double Timer::GetDeltaTimeSec()
	{
		return (float)GetDeltaTimeMs() * 0.001f;
	}

	int Timer::GetFPS()
	{
		return m_Fps;
	}

    void Timer::SleepSec(int time)
    {
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }

    void Timer::SleepMill(int time)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time));
	}

	void Timer::SleepMicro(int time)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(time));
	}

	void Timer::SleepNano(int time)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(time));
	}

	void Timer::Delay(double millSec, std::function<void()> callback)
	{
		s_DelayCallbacks.push_back({ GetTimeMill(), millSec, callback });
	}

    void Timer::AddGpuTimerSample(float time)
    {
		m_FrameGPUTime_C += time;
    }

    void Timer::AddCpuTimerSample(float time)
    {
        m_FrameCPUTime_C += time;
    }

    void Timer::AddTotalTimerSample(float time)
    {
        m_FrameTimePeriod_C += time;
    }

}