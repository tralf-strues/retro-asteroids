# Retro Asteroids

Retro Asteroids is a game inspired by [Asteroids](https://en.wikipedia.org/wiki/Asteroids_(video_game)) and [Geometry Wars](https://en.wikipedia.org/wiki/Geometry_Wars) games.

## Build

The only dependency is X11. The supported compilers are GCC, Clang and AppleClang.

There are also several cmake configuration options:

Option                     | Description                                                             | Default
---------------------------|-------------------------------------------------------------------------|--------
RA_ENABLE_WARNINGS         | Enable warnings when compiling                                          | ON
RA_BUILD_WITH_ASAN         | Enable address sanitizer                                                | OFF
RA_BUILD_WITH_TSAN         | Enable thread sanitizer                                                 | OFF
RA_ENABLE_COLOR_LOG_OUTPUT | Whether to enable colored terminal output (using ANSI escape sequences) | ON

### Linux (Ubuntu)
Get dependencies and clone the repository:
```bash
sudo apt install {g++|clang} cmake libx11-dev
git clone https://github.com/tralf-strues/retro-asteroids
```

Build:
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE={Debug|Release} ..
make
```

### MacOS
Get dependencies and clone the repository:
```bash
brew install --cask xquartz
brew install {gcc|clang} cmake
git clone https://github.com/tralf-strues/retro-asteroids
```

Build:
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE={Debug|Release} ..
make
```
