/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Keyboard.hpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <cstdint>

namespace ra::input {

enum class Key : uint32_t {
  Esc,
  Space,
  Left,
  Up,
  Right,
  Down,

  Enter
};

bool CheckKey(Key key);

}  // namespace ra::input