#include "Log.h"

// Create logs directory
#include <filesystem>

// spdlog includes
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::init()
{
	const std::string pattern = "[%T.%e] Thread: %-5t %=6n: %^[%=7l] %v%$";

	std::filesystem::create_directory("logs");

	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_level(spdlog::level::trace);
	console_sink->set_pattern(pattern);

	auto file_sink_system = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/system.txt", true);
	file_sink_system->set_level(spdlog::level::trace);
	file_sink_system->set_pattern(pattern);

	spdlog::set_pattern(pattern);

	s_Logger = std::make_shared<spdlog::logger>("SYSTEM", std::initializer_list<spdlog::sink_ptr>{console_sink, file_sink_system});;
	s_Logger->set_level(spdlog::level::trace);
}

void Log::flush()
{
	LOG_INFO("FLUSHING");

	s_Logger->flush();
}

void Log::setLevel(spdlog::level::level_enum level)
{
	s_Logger->set_level(level);
}