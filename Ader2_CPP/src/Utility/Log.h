#pragma once
#include "spdlog/spdlog.h"

/**
 * The logger class used by the engine powered by spdlog
 */
class Log
{
public:
	/**
	 * Initializes logger
	 */
	static void init();

	/**
	 * Flush all logs to files
	 */
	static void flush();

	/**
	 * Set the level of debug messages to be printed to the console
	 *
	 * @param level The spdlog level of the message
	 */
	static void setLevel(spdlog::level::level_enum level);

	inline static std::shared_ptr<spdlog::logger>& getLogger() { return s_Logger; }
private:
	/// System logger logs messages to console rather than the GUI log
	static std::shared_ptr<spdlog::logger> s_Logger;
};

// GUI log macros
#define LOG_TRACE(...)       ::Log::getLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)        ::Log::getLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)        ::Log::getLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)       ::Log::getLogger()->error(__VA_ARGS__)