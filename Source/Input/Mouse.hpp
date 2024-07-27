/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Mouse.hpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec2.hpp>

#include <optional>

namespace ra::input {

enum class MouseButton : uint32_t {
  Left,
  Middle,
  Right,
  WheelUp,
  WheelDown
};

bool CheckMouseButton(MouseButton button);

std::optional<math::Vec2u> GetCursorPosition();

}  // namespace ra::input