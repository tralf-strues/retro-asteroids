/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Log.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Utils/StringLiteral.hpp>

#include <cstdint>
#include <cstdio>
#include <string_view>

namespace ra::utils {

enum class LogLevel : uint32_t {
  Info,
  Warning,
  Error,
  Fatal
};

namespace detail {

constexpr const char* ToString(LogLevel level) {
  switch (level) {
    case ra::utils::LogLevel::Info:    { return "I"; }
    case ra::utils::LogLevel::Warning: { return "W"; }
    case ra::utils::LogLevel::Error:   { return "E"; }
    case ra::utils::LogLevel::Fatal:   { return "F"; }
    default:                           { return "?"; }
  }
}

constexpr const char* MessageLevelColor(LogLevel level) {
  switch (level) {
    case ra::utils::LogLevel::Info:    { return ""; }
    case ra::utils::LogLevel::Warning: { return ";38;5;214"; }
    case ra::utils::LogLevel::Error:   { return ";38;5;160"; }
    case ra::utils::LogLevel::Fatal:   { return ";38;5;196"; }
    default:                           { return ""; }
  }
}

}  // namespace detail

template <StringLiteral FormatString, typename... Args>
void LogMessage(LogLevel level, std::string_view location, Args&&... args) {
#ifdef RA_ENABLE_COLOR_LOG_OUTPUT
  std::printf("\033[1%sm", detail::MessageLevelColor(level));
  std::printf("[%s][%s] ", detail::ToString(level), location.data());
  std::printf("\033[0m");

  std::printf("\033[%sm", detail::MessageLevelColor(level));
  if constexpr(sizeof...(Args) > 0U) {
    std::printf(FormatString.data, std::forward<Args>(args)...);
  } else {
    std::printf("%s", FormatString.data);
  }
  std::printf("\033[0m\n");
#else
  std::printf("[%s][%s] ", detail::ToString(level), location.data());
  if constexpr(sizeof...(Args) > 0U) {
    std::printf(FormatString.data, std::forward<Args>(args)...);
  } else {
    std::printf("%s", FormatString.data);
  }
  std::printf("\n");
#endif
}

}  // namespace ra::utils

#define RA_LINE_TO_STR(x) RA_TO_STR(x)
#define RA_TO_STR(x) #x

#ifdef __FILE_NAME__
#define RA_FILE_NAME __FILE_NAME__
#else
#define RA_FILE_NAME __FILE__
#endif

#define RA_LOG_INFO(fmt, ...)                               \
  ::ra::utils::LogMessage<::ra::utils::StringLiteral(fmt)>( \
      ::ra::utils::LogLevel::Info, RA_FILE_NAME ":" RA_LINE_TO_STR(__LINE__) __VA_OPT__(, ) __VA_ARGS__)

#define RA_LOG_WARN(fmt, ...)                               \
  ::ra::utils::LogMessage<::ra::utils::StringLiteral(fmt)>( \
      ::ra::utils::LogLevel::Warning, RA_FILE_NAME ":" RA_LINE_TO_STR(__LINE__) __VA_OPT__(, ) __VA_ARGS__)

#define RA_LOG_ERROR(fmt, ...)                              \
  ::ra::utils::LogMessage<::ra::utils::StringLiteral(fmt)>( \
      ::ra::utils::LogLevel::Error, RA_FILE_NAME ":" RA_LINE_TO_STR(__LINE__) __VA_OPT__(, ) __VA_ARGS__)

#define RA_LOG_FATAL(fmt, ...)                              \
  ::ra::utils::LogMessage<::ra::utils::StringLiteral(fmt)>( \
      ::ra::utils::LogLevel::Fatal, RA_FILE_NAME ":" RA_LINE_TO_STR(__LINE__) __VA_OPT__(, ) __VA_ARGS__)