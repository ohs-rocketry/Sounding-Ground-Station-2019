#pragma once

#include <memory>
#include <spdlog/spdlog.h>

#define GS_TRACE(...)		Log::GetLogger()->trace(__VA_ARGS__)
#define GS_DEBUG(...)		Log::GetLogger()->debug(__VA_ARGS__)
#define GS_INFO(...)		Log::GetLogger()->info(__VA_ARGS__)
#define GS_WARN(...)		Log::GetLogger()->warn(__VA_ARGS__)
#define GS_ERROR(...)		Log::GetLogger()->error(__VA_ARGS__)
#define GS_CRITICAL(...)	Log::GetLogger()->critical(__VA_ARGS__)

class Log {
public:
	static void Init();
	static inline std::shared_ptr<spdlog::logger> GetLogger() { return logger; }

private:
	static std::shared_ptr<spdlog::logger> logger;
};