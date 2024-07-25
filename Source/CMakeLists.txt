message("-- Configuring Retro-Asteroids")

add_executable(retro-asteroids)

# Move executable to the root project folder
set_target_properties(retro-asteroids PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../")

include("../Cmake/CompileOptions.cmake")
message("-- Game flags (RA_COMPILE_FLAGS): ${RA_COMPILE_FLAGS}")
message("-- Game flags (RA_LINK_FLAGS): ${RA_LINK_FLAGS}")

target_compile_options(retro-asteroids PRIVATE ${RA_COMPILE_FLAGS})
target_link_options(retro-asteroids PRIVATE ${RA_LINK_FLAGS})

file(GLOB_RECURSE RETRO_ASTEROIDS_SOURCE_PRIVATE
  *.hpp
  *.h
  *.cpp
  *.c
)

target_include_directories(retro-asteroids
  PRIVATE
    .
)

target_sources(retro-asteroids
  PRIVATE
    ${RETRO_ASTEROIDS_SOURCE_PRIVATE}
)

find_package(X11 REQUIRED)

target_include_directories(retro-asteroids PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(retro-asteroids PRIVATE ${X11_LIBRARIES})
