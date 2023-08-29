#ifndef Logger_h__
#define Logger_h__

#include "spdlog/spdlog.h"

namespace rush
{

	class Logger
	{
	public:
		static void Init(const char* logName, const char* segment);

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger; 
	};

}

#ifdef _DEBUG
	#define LOG_TRACE(...) ::rush::Logger::GetLogger()->trace(__VA_ARGS__)
	#define LOG_INFO(...) ::rush::Logger::GetLogger()->info(__VA_ARGS__)
	#define LOG_DBG(...) ::rush::Logger::GetLogger()->debug(__VA_ARGS__)
	#define LOG_WARN(...) ::rush::Logger::GetLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...) ::rush::Logger::GetLogger()->error(__VA_ARGS__)
	#define LOG_CRITICAL(...) ::rush::Logger::GetLogger()->critical(__VA_ARGS__); RUSH_DEBUGBREAK()
#else
	#define LOG_TRACE(...) 
	#define LOG_INFO(...) ::rush::Logger::GetLogger()->info(__VA_ARGS__)
	#define LOG_DBG(...) 
	#define LOG_WARN(...) ::rush::Logger::GetLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...) ::rush::Logger::GetLogger()->error(__VA_ARGS__)
	#define LOG_CRITICAL(...) ::rush::Logger::GetLogger()->critical(__VA_ARGS__);
#endif

#endif // Logger_h__
