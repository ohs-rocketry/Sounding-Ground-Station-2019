
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> Log::logger;

void Log::Init() {
	std::string consolePattern = "%^[%T] %l %n: %$%v", filePattern = "[%c] [%l] %n: %v";

	auto stdOut = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#ifdef GS_DEBUG
	stdOut->set_level(spdlog::level::trace);
#else
	stdOut->set_level(spdlog::level::warn);
#endif
	stdOut->set_pattern(consolePattern);

	auto file = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
	file->set_level(spdlog::level::trace);
	file->set_pattern(filePattern);

	logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("Ground Station", { stdOut, file }));
	logger->set_level(spdlog::level::trace);

	GS_INFO("Ground Station Logging Initalization Complete");
}
