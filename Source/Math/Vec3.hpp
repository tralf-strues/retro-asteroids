/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Vec3.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec2.hpp>
#include <Utils/Assert.hpp>

namespace ra::math {

template <typename T>
struct Vec3 {
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
      union {
        T x;
        T r;
      };

      union {
        T y;
        T g;
      };

      union {
        T z;
        T b;
      };
    };

    T elems[3U];
  };

  # if defined(RA_COMPILER_GCC)
  #   pragma GCC diagnostic pop
  # elif defined(RA_COMPILER_CLANG)
  #   pragma clang diagnostic pop
  # elif defined(RA_COMPILER_MSVC)
  #   pragma warning(pop)
  # endif

  inline constexpr Vec3() = default;
  inline constexpr explicit Vec3(const T& xyz) : x(xyz), y(xyz), z(xyz) {}
  inline constexpr Vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}
  inline constexpr Vec3(const Vec2<T>& vec2, const T& z) : x(vec2.x), y(vec2.y), z(z) {}

  template <typename U>
  inline constexpr explicit Vec3(const Vec3<U>& other)
      : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {}

  inline constexpr T& operator[](size_t element);
  inline constexpr const T& operator[](size_t element) const;

  inline constexpr Vec3& operator+=(const Vec3& rhs);
  inline constexpr Vec3& operator-=(const Vec3& rhs);
  inline constexpr Vec3& operator*=(const Vec3& rhs);
  inline constexpr Vec3& operator*=(T scalar);
  inline constexpr Vec3& operator/=(T scalar);

  inline constexpr explicit operator Vec2<T>() const;
};

using Vec3u = Vec3<uint32_t>;
using Vec3i = Vec3<int32_t>;
using Vec3f = Vec3<float>;

template <typename T>
inline constexpr T& Vec3<T>::operator[](size_t element) {
  RA_ASSERT(element < 3U, "Vec3 element access out of bounds, element = %zu", element);
  return elems[element];
}

template <typename T>
inline constexpr const T& Vec3<T>::operator[](size_t element) const {
  RA_ASSERT(element < 3U, "Vec3 element access out of bounds, element = %zu", element);
  return elems[element];
}

template <typename T>
inline constexpr Vec3<T>& Vec3<T>::operator+=(const Vec3<T>& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

template <typename T>
inline constexpr Vec3<T>& Vec3<T>::operator-=(const Vec3<T>& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

template <typename T>
inline constexpr Vec3<T>& Vec3<T>::operator*=(const Vec3<T>& rhs) {
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  return *this;
}

template <typename T>
inline constexpr Vec3<T>& Vec3<T>::operator*=(T scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

template <typename T>
inline constexpr Vec3<T>& Vec3<T>::operator/=(T scalar) {
  RA_ASSERT(scalar != 0, "Division by zero");

  x /= scalar;
  y /= scalar;
  z /= scalar;

  return *this;
}

template <typename T>
inline constexpr Vec3<T>::operator Vec2<T>() const {
  return Vec2<T>(x, y);
}

template <typename T>
inline constexpr Vec3<T> operator+(const Vec3<T>& lhs, const Vec3<T>& rhs) {
  Vec3<T> sum(lhs);
  sum += rhs;
  return sum;
}

template <typename T>
inline constexpr Vec3<T> operator-(const Vec3<T>& lhs, const Vec3<T>& rhs) {
  Vec3<T> dif(lhs);
  dif -= rhs;
  return dif;
}

template <typename T>
inline constexpr Vec3<T> operator*(const Vec3<T>& lhs, const Vec3<T>& rhs) {
  Vec3<T> mul(lhs);
  mul *= rhs;
  return mul;
}

template <typename T>
inline constexpr Vec3<T> operator*(const Vec3<T>& vector, T scalar) {
  Vec3<T> mul(vector);
  mul *= scalar;
  return mul;
}

template <typename T>
inline constexpr Vec3<T> operator*(T scalar, const Vec3<T>& vector) {
  return vector * scalar;
}

template <typename T>
inline constexpr Vec3<T> operator/(const Vec3<T>& vector, T scalar) {
  Vec3<T> ratio(vector);
  ratio /= scalar;
  return ratio;
}

template <typename T>
inline constexpr Vec3<T> operator-(const Vec3<T>& vector) {
  return vector * (-1.0f);
}

template <typename T>
inline constexpr float LengthSquared(const Vec3<T>& vector) {
  return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

template <typename T>
inline constexpr float Length(const Vec3<T>& vector) {
  return std::sqrtf(LengthSquared(vector));
}

template <typename T>
inline constexpr Vec3<T> Normalize(const Vec3<T>& vector) {
  float len = Length(vector);
  if (len == 0.0f) {
    return Vec3<T>(0.0f);
  }

  return vector / len;
}

template <typename T>
inline constexpr T Dot(const Vec3<T>& lhs, const Vec3<T>& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

}  // namespace ra::math