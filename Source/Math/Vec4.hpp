/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Vec4.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec3.hpp>

namespace ra::math {

template <typename T>
struct Vec4 {
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

      union {
        T w;
        T a;
      };
    };

    Vec3<T> xyz;
    Vec3<T> rgb;

    T elems[4U];
  };

  # if defined(RA_COMPILER_GCC)
  #   pragma GCC diagnostic pop
  # elif defined(RA_COMPILER_CLANG)
  #   pragma clang diagnostic pop
  # elif defined(RA_COMPILER_MSVC)
  #   pragma warning(pop)
  # endif

  inline constexpr Vec4() = default;
  inline constexpr explicit Vec4(const T& xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
  inline constexpr Vec4(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) {}
  inline constexpr Vec4(const Vec3<T>& vec3, const T& w) : x(vec3.x), y(vec3.y), z(vec3.z), w(w) {}

  template <typename U>
  inline constexpr explicit Vec4(const Vec4<U>& other)
      : x(static_cast<T>(other.x)),
        y(static_cast<T>(other.y)),
        z(static_cast<T>(other.z)),
        w(static_cast<T>(other.w)) {}

  inline constexpr T& operator[](size_t element);
  inline constexpr const T& operator[](size_t element) const;

  inline constexpr Vec4& operator+=(const Vec4& rhs);
  inline constexpr Vec4& operator-=(const Vec4& rhs);
  inline constexpr Vec4& operator*=(const Vec4& rhs);
  inline constexpr Vec4& operator*=(T scalar);
  inline constexpr Vec4& operator/=(T scalar);

  inline constexpr explicit operator Vec3<T>() const;
};

using Vec4u = Vec4<uint32_t>;
using Vec4i = Vec4<int32_t>;
using Vec4f = Vec4<float>;

template <typename T>
inline constexpr T& Vec4<T>::operator[](size_t element) {
  RA_ASSERT(element < 4U, "Vec4 element access out of bounds, element = %zu", element);
  return elems[element];
}

template <typename T>
inline constexpr const T& Vec4<T>::operator[](size_t element) const {
  RA_ASSERT(element < 4U, "Vec4 element access out of bounds, element = %zu", element);
  return elems[element];
}

template <typename T>
inline constexpr Vec4<T>& Vec4<T>::operator+=(const Vec4<T>& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

template <typename T>
inline constexpr Vec4<T>& Vec4<T>::operator-=(const Vec4<T>& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

template <typename T>
inline constexpr Vec4<T>& Vec4<T>::operator*=(const Vec4<T>& rhs) {
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  w *= rhs.w;
  return *this;
}

template <typename T>
inline constexpr Vec4<T>& Vec4<T>::operator*=(T scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  w *= scalar;
  return *this;
}

template <typename T>
inline constexpr Vec4<T>& Vec4<T>::operator/=(T scalar) {
  RA_ASSERT(scalar != 0, "Division by zero");

  x /= scalar;
  y /= scalar;
  z /= scalar;
  w /= scalar;

  return *this;
}

template <typename T>
inline constexpr Vec4<T>::operator Vec3<T>() const {
  return Vec3<T>(x, y, z);
}

template <typename T>
inline constexpr Vec4<T> operator+(const Vec4<T>& lhs, const Vec4<T>& rhs) {
  Vec4<T> sum(lhs);
  sum += rhs;
  return sum;
}

template <typename T>
inline constexpr Vec4<T> operator-(const Vec4<T>& lhs, const Vec4<T>& rhs) {
  Vec4<T> dif(lhs);
  dif -= rhs;
  return dif;
}

template <typename T>
inline constexpr Vec4<T> operator*(const Vec4<T>& lhs, const Vec4<T>& rhs) {
  Vec4<T> mul(lhs);
  mul *= rhs;
  return mul;
}

template <typename T>
inline constexpr Vec4<T> operator*(const Vec4<T>& vector, T scalar) {
  Vec4<T> mul(vector);
  mul *= scalar;
  return mul;
}

template <typename T>
inline constexpr Vec4<T> operator*(T scalar, const Vec4<T>& vector) {
  return vector * scalar;
}

template <typename T>
inline constexpr Vec4<T> operator/(const Vec4<T>& vector, T scalar) {
  Vec4<T> ratio(vector);
  ratio /= scalar;
  return ratio;
}

template <typename T>
inline constexpr Vec4<T> operator-(const Vec4<T>& vector) {
  return vector * (-1.0f);
}

template <typename T>
inline constexpr float LengthSquared(const Vec4<T>& vector) {
  return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
}

template <typename T>
inline constexpr float Length(const Vec4<T>& vector) {
  return std::sqrtf(LengthSquared(vector));
}

template <typename T>
inline constexpr Vec4<T> Normalize(const Vec4<T>& vector) {
  float len = Length(vector);
  if (len == 0.0f) {
    return Vec4<T>(0.0f);
  }

  return vector / len;
}

template <typename T>
inline constexpr T Lerp(const T& lhs, const T& rhs, float t) {
  return lhs + t * (rhs - lhs);
}

}  // namespace ra::math