set(CMAKE_CXX_STANDARD 20)

message("-- Compiler ID C++: ${CMAKE_CXX_COMPILER_ID}")

# Prepare flag variables and declare functions
set(RA_COMPILE_FLAGS "")
set(RA_LINK_FLAGS "")

function(add_ra_compile_flags flags)
  set(RA_COMPILE_FLAGS "${RA_COMPILE_FLAGS} ${flags}" PARENT_SCOPE)
endfunction()

function(add_ra_link_flags flags)
  set(RA_LINK_FLAGS "${RA_LINK_FLAGS} ${flags}" PARENT_SCOPE)
endfunction()

# C++ version
add_ra_compile_flags("-std=c++20")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_ra_compile_flags("-stdlib=libc++")
  add_ra_link_flags("-stdlib=libc++")
endif()

# Debug/Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_ra_compile_flags("-DRA_DEBUG_MODE")
  add_ra_compile_flags("-g -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_ra_compile_flags("-DRA_RELEASE_MODE")

  string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  string(REPLACE "/D_DEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

  add_ra_compile_flags("-O2")
endif()

# Sanitizers
option(RA_BUILD_WITH_ASAN "Enable address sanitizer (Linux and MacOS only)" OFF)
option(RA_BUILD_WITH_TSAN "Enable thread sanitizer (Linux and MacOS only)" OFF)

if(RA_BUILD_WITH_ASAN AND RA_BUILD_WITH_TSAN)
  message(FATAL_ERROR "Asan and tsan can not be enabled at the same time")
endif()

if(RA_BUILD_WITH_ASAN)
  message("-- Address sanitizer enabled")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_ra_compile_flags("/fsanitize=address")
    add_ra_link_flags("/fsanitize=address")
  else()
    add_ra_compile_flags("-fsanitize=address")
    add_ra_link_flags("-fsanitize=address")
  endif()
else()
  message("-- Address sanitizer disabled")
endif()

if(RA_BUILD_WITH_TSAN)
  message("-- Thread sanitizer enabled")
  add_ra_compile_flags("-fsanitize=thread")
  add_ra_link_flags("-fsanitize=thread")
else()
  message("-- Thread sanitizer disabled")
endif()

# Warnings
option(RA_ENABLE_WARNINGS "Enable warnings when compiling" ON)

if(RA_ENABLE_WARNINGS)
  add_ra_compile_flags("-Wall")
  add_ra_compile_flags("-Wextra")
  add_ra_compile_flags("-Wpedantic")

  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_ra_compile_flags("-Wno-changes-meaning")
  endif()
endif()

# Additional macro definitions
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_ra_compile_flags("-DRA_COMPILER_GCC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  add_ra_compile_flags("-DRA_COMPILER_CLANG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_ra_compile_flags("/DRA_COMPILER_MSVC")
else()
  add_ra_compile_flags("-DRA_COMPILER_UNDEFINED")
endif()

option(RA_ENABLE_COLOR_LOG_OUTPUT "Whether to enable colored terminal output (using ANSI escape sequences)" ON)

if(RA_ENABLE_COLOR_LOG_OUTPUT)
  add_ra_compile_flags("-DRA_ENABLE_COLOR_LOG_OUTPUT")
endif()

# Convert to have ; as separators
string(REPLACE " " ";" RA_COMPILE_FLAGS "${RA_COMPILE_FLAGS}")
string(REPLACE " " ";" RA_LINK_FLAGS "${RA_LINK_FLAGS}")
