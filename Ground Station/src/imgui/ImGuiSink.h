#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/details/console_globals.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/sink.h>

#include "util/Console.h"

#include <mutex>
#include <imgui.h>

using namespace spdlog;
using namespace sinks;

class imgui_sink : public sink {

public:

	void log(const details::log_msg &msg) final override;
	void flush() final override;
	void set_pattern(const std::string &pattern) override final;
	void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override final;

private:

	std::mutex mutex;
};

