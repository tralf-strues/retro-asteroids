/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Mouse.cpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#include <Template/Engine.h>
#include <Input/Mouse.hpp>
#include <Utils/Log.hpp>

namespace ra::input {

bool CheckMouseButton(MouseButton button) {
  return is_mouse_button_pressed(static_cast<int>(button));
}

std::optional<math::Vec2u> GetCursorPosition() {
  auto pos = math::Vec2i(get_cursor_x(), get_cursor_y());
  if (pos.x < 0 || pos.x >= SCREEN_WIDTH || pos.y < 0 || pos.y >= SCREEN_HEIGHT) {
    return std::nullopt;
  }

  return math::Vec2u(pos);
}

}  // namespace ra::input