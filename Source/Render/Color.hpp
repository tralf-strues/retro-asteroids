/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Color.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec4.hpp>

namespace ra::render {

class Color {
 public:
  inline constexpr Color() = default;

  inline constexpr explicit Color(uint32_t value);
  inline constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);

  inline constexpr explicit Color(const math::Vec4f& normalized);

  inline constexpr uint8_t R() const;
  inline constexpr uint8_t G() const;
  inline constexpr uint8_t B() const;
  inline constexpr uint8_t A() const;

  inline constexpr uint32_t Value() const;

  inline constexpr explicit operator math::Vec4f() const;

 private:
  uint32_t value_{0x0U};  // Format: ARGB
};

inline constexpr Color::Color(uint32_t value) : value_(value) {}

inline constexpr Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : Color((a << 24u) + (r << 16u) + (g << 8u) + b) {}

inline constexpr Color::Color(const math::Vec4f& normalized)
    : Color(normalized.r * 0xFF, normalized.g * 0xFF, normalized.b * 0xFF, normalized.a * 0xFF) {}

namespace detail {

inline constexpr uint32_t ExtractByte(uint32_t value, uint8_t byte_idx) {
  return (value & (0xFFu << (8u * byte_idx))) >> (8u * byte_idx);
}

}  // namespace detail

inline constexpr uint8_t Color::R() const { return detail::ExtractByte(value_, 2U); }
inline constexpr uint8_t Color::G() const { return detail::ExtractByte(value_, 1U); }
inline constexpr uint8_t Color::B() const { return detail::ExtractByte(value_, 0U); }
inline constexpr uint8_t Color::A() const { return detail::ExtractByte(value_, 3U); }

inline constexpr uint32_t Color::Value() const { return value_; }

inline constexpr Color::operator math::Vec4f() const {
  return math::Vec4f(R() / 255.0f, G() / 255.0f, B() / 255.0f, A() / 255.0f);
}

}  // namespace ra::render