/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Keyboard.cpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#include <Template/Engine.h>
#include <Input/Keyboard.hpp>

namespace ra::input {

bool CheckKey(Key key) {
  return is_key_pressed(static_cast<int>(key));
}

}  // namespace ra::input