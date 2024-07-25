/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Vec2.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <cmath>

namespace ra::math {

template <typename T>
struct Vec2 {
  # if defined(RA_COMPILER_GCC)
  #   pragma GCC diagnostic push
  #   pragma GCC diagnostic ignored "-Wpedantic"
  # elif defined(RA_COMPILER_CLANG)
  #   pragma clang diagnostic push
  #   pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
  #   pragma clang diagnostic ignored "-Wnested-anon-types"
  # elif defined(RA_COMPILER_MSVC)
  #   pragma warning(push)
  #   pragma warning(disable: 4201)
  # endif

  union {
    struct {
      T x;
      T y;
    };

    T elems[2U];
  };

  # if defined(RA_COMPILER_GCC)
  #   pragma GCC diagnostic pop
  # elif defined(RA_COMPILER_CLANG)
  #   pragma clang diagnostic pop
  # elif defined(RA_COMPILER_MSVC)
  #   pragma warning(pop)
  # endif

  inline constexpr Vec2() = default;
  inline constexpr explicit Vec2(const T& xy) : x(xy), y(xy) {}
  inline constexpr Vec2(const T& x, const T& y) : x(x), y(y) {}

  template <typename U>
  inline constexpr explicit Vec2(const Vec2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

  inline constexpr Vec2& operator+=(const Vec2& rhs);
  inline constexpr Vec2& operator-=(const Vec2& rhs);
  inline constexpr Vec2& operator*=(const Vec2& rhs);
  inline constexpr Vec2& operator*=(float scalar);
  inline constexpr Vec2& operator/=(const Vec2& rhs);
  inline constexpr Vec2& operator/=(float scalar);
};

using Vec2u = Vec2<uint32_t>;
using Vec2i = Vec2<int32_t>;
using Vec2f = Vec2<float>;

template <typename T>
inline constexpr Vec2<T>& Vec2<T>::operator+=(const Vec2<T>& rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

template <typename T>
inline constexpr Vec2<T>& Vec2<T>::operator-=(const Vec2<T>& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

template <typename T>
inline constexpr Vec2<T>& Vec2<T>::operator*=(const Vec2<T>& rhs) {
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}

template <typename T>
inline constexpr Vec2<T>& Vec2<T>::operator*=(float scalar) {
  x *= scalar;
  y *= scalar;
  return *this;
}

template <typename T>
inline constexpr Vec2<T>& Vec2<T>::operator/=(const Vec2<T>& rhs) {
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

template <typename T>
inline constexpr Vec2<T>& Vec2<T>::operator/=(float scalar) {
  x /= scalar;
  y /= scalar;
  return *this;
}

template <typename T>
inline constexpr bool operator==(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
inline constexpr Vec2<T> operator+(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  Vec2<T> sum(lhs);
  sum += rhs;
  return sum;
}

template <typename T>
inline constexpr Vec2<T> operator-(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  Vec2<T> dif(lhs);
  dif -= rhs;
  return dif;
}

template <typename T>
inline constexpr Vec2<T> operator*(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  Vec2<T> mul(lhs);
  mul *= rhs;
  return mul;
}

template <typename T>
inline constexpr Vec2<T> operator*(const Vec2<T>& vector, T scalar) {
  Vec2<T> mul(vector);
  mul *= scalar;
  return mul;
}

template <typename T>
inline constexpr Vec2<T> operator*(T scalar, const Vec2<T>& vector) {
  return vector * scalar;
}

template <typename T>
inline constexpr Vec2<T> operator/(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  Vec2<T> ratio(lhs);
  ratio /= rhs;
  return ratio;
}

template <typename T>
inline constexpr Vec2<T> operator/(const Vec2<T>& vector, T scalar) {
  Vec2<T> ratio(vector);
  ratio /= scalar;
  return ratio;
}

template <typename T>
inline constexpr Vec2<T> operator-(const Vec2<T>& vector) {
  return vector * (-1.0f);
}

template <typename T>
inline constexpr float LengthSquared(const Vec2<T>& vector) {
  return vector.x * vector.x + vector.y * vector.y;
}

template <typename T>
inline constexpr float Length(const Vec2<T>& vector) {
  return std::sqrtf(LengthSquared(vector));
}

template <typename T>
inline constexpr Vec2<T> Normalize(const Vec2<T>& vector) {
  float len = length(vector);
  if (len == 0.0f) {
    return Vec2<T>(0.0f);
  }

  return vector / len;
}

template <typename T>
inline constexpr T Dot(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

}  // namespace ra::math
