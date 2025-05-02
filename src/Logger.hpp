#pragma once

#include "magic_enum/magic_enum.hpp"
#include <array>
#include <format>
#include <iostream>
#include <string_view>

namespace Enoki
{
	enum class LogLevel : unsigned char
	{
		Info,
		Debug,
		Trace,
		Warn,
		Error,
	};

	namespace Internal
	{
		static constexpr auto LogLevelNames = magic_enum::enum_names<LogLevel>();

		static constexpr std::string_view GetLogLevelName(LogLevel level)
		{
			return LogLevelNames[(unsigned char)(level)];
		}
	} // namespace Internal

	// template <LogLevel Level, class... Args>
	// static void Log(std::string_view category, const std::format_string<Args...> fmt, Args&&... args)
	// {
	// 	std::cout << std::format("{} |{}| : ", magic_enum::enum_name<Level>, category) << std::format(fmt, std::forward<Args>(args)...)
	// 			  << std::endl;
	// }

	// static void LogInfo(std::string_view category, const char* string, auto... args)
	// {
	// 	Log<LogLevel::Info>(category, string, args...);
	// }
	// static void LogDebug(std::string_view category, const char* string, auto... args)
	// {
	// 	Log<LogLevel::Debug>(category, string, args...);
	// }
	// static void LogTrace(std::string_view category, const char* string, auto... args)
	// {
	// 	Log<LogLevel::Trace>(category, string, args...);
	// }
	// static void LogWarn(std::string_view category, const char* string, auto... args)
	// {
	// 	Log<LogLevel::Warn>(category, string, args...);
	// }
	// static void LogError(std::string_view category, const char* string, auto... args)
	// {
	// 	Log<LogLevel::Error>(category, string, args...);
	// }
} // namespace Enoki

#define ENOKI_LOG(category, level, fmt, ...)                                                                                               \
	std::cout << std::format("{} |{}| : ", Enoki::Internal::GetLogLevelName(level), #category) << std::format(fmt, __VA_ARGS__)            \
			  << std::endl;

#define ENOKI_INFO(category, fmt, ...) ENOKI_LOG(category, Enoki::LogLevel::Info, fmt, __VA_ARGS__)
#define ENOKI_DEBUG(category, fmt, ...) ENOKI_LOG(category, Enoki::LogLevel::Debug, fmt, __VA_ARGS__)
#define ENOKI_TRACE(category, fmt, ...) ENOKI_LOG(category, Enoki::LogLevel::Trace, fmt, __VA_ARGS__)
#define ENOKI_WARN(category, fmt, ...) ENOKI_LOG(category, Enoki::LogLevel::Warn, fmt, __VA_ARGS__)
#define ENOKI_ERROR(category, fmt, ...) ENOKI_LOG(category, Enoki::LogLevel::Error, fmt, __VA_ARGS__)