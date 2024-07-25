/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Assert.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Utils/Log.hpp>

#include <exception>

#if defined(RA_DEBUG_MODE)

#define RA_ASSERT(condition, fmt, ...) \
  if (!(condition)) {                  \
    RA_LOG_FATAL(fmt, __VA_ARGS__);    \
    std::terminate();                  \
  }

#else

#define RA_ASSERT(condition, ...)

#endif