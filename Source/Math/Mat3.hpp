/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Mat3.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec3.hpp>

namespace ra::math {

template <typename T>
struct Mat3 {
  T elements[3U * 3U];

  inline constexpr T*       operator[](size_t row);
  inline constexpr const T* operator[](size_t row) const;

  inline constexpr Mat3& operator+=(const Mat3& second);
  inline constexpr Mat3& operator-=(const Mat3& second);
  inline constexpr Mat3& operator*=(const Mat3& second);

  inline constexpr Mat3& operator*=(T scalar);
};

using Mat3f = Mat3<float>;

template <typename T>
inline constexpr T* Mat3<T>::operator[](size_t row) {
  RA_ASSERT(row < 3U, "Mat3 row access out of bounds, row = %zu", row);
  return &elements[row * 3U];
}

template <typename T>
inline constexpr const T* Mat3<T>::operator[](size_t row) const {
  RA_ASSERT(row < 3U, "Mat3 row access out of bounds, row = %zu", row);
  return &elements[row * 3U];
}

template <typename T>
inline constexpr Mat3<T>& Mat3<T>::operator+=(const Mat3<T>& second) {
  for (size_t i = 0U; i < 3U * 3U; i++) {
    elements[i] += second.elements[i];
  }

  return *this;
}

template <typename T>
inline constexpr Mat3<T>& Mat3<T>::operator-=(const Mat3<T>& second) {
  for (size_t i = 0U; i < 3U * 3U; i++) {
    elements[i] -= second.elements[i];
  }

  return *this;
}

template <typename T>
inline constexpr Mat3<T>& Mat3<T>::operator*=(const Mat3<T>& second) {
  Mat3<T> old(*this);

  for (size_t row = 0U; row < 3U; row++) {
    for (size_t col = 0U; col < 3U; col++) {
      T result = 0;

      for (size_t i = 0U; i < 3U; i++) {
        result += old.elements[row * 3U + i] * second.elements[i * 3U + col];
      }

      elements[row * 3U + col] = result;
    }
  }

  return *this;
}

template <typename T>
inline constexpr Mat3<T>& Mat3<T>::operator*=(T scalar) {
  for (auto& element : elements) {
    element *= scalar;
  }
}

template <typename T>
inline constexpr Mat3<T> operator+(const Mat3<T>& first, const Mat3<T>& second) {
  Mat3<T> sum(first);
  sum += second;

  return sum;
}

template <typename T>
inline constexpr Mat3<T> operator-(const Mat3<T>& first, const Mat3<T>& second) {
  Mat3<T> dif(first);
  dif -= second;

  return dif;
}

template <typename T>
inline constexpr Mat3<T> operator*(const Mat3<T>& first, const Mat3<T>& second) {
  Mat3<T> product(first);
  product *= second;

  return product;
}

template <typename T>
inline constexpr Vec3<T> operator*(const Mat3<T>& matrix, const Vec3<T>& vector) {
  Vec3<T> product{0, 0, 0};

  for (size_t row = 0U; row < 3U; row++) {
    for (size_t col = 0U; col < 3U; col++) {
      product[row] += matrix[row][col] * vector[col];
    }
  }

  return product;
}

template <typename T>
inline constexpr Mat3<T> operator*(const Mat3<T>& matrix, T scalar) {
  Mat3<T> product(matrix);
  product *= scalar;

  return product;
}

template <typename T>
inline constexpr Mat3<T> Transpose(const Mat3<T>& matrix) {
  Mat3<T> transpose;

  for (size_t row = 0U; row < 3U; row++) {
    for (size_t col = 0U; col < 3U; col++) {
      transpose[row][col] = matrix[col][row];
    }
  }

  return transpose;
}

template <typename T>
inline constexpr Mat3<T> Identity(T diagonal = 1) {
  Mat3<T> matrix;

  for (size_t row = 0U; row < 3U; row++) {
    for (size_t col = 0U; col < 3U; col++) {
      matrix[row][col] = diagonal;
    }
  }

  return matrix;
}

inline constexpr float Determinant(const Mat3f& matrix) {
  return matrix[0U][0U] * (matrix[1U][1U] * matrix[2U][2U] - matrix[1U][2U] * matrix[2U][1U]) -
         matrix[0U][1U] * (matrix[1U][0U] * matrix[2U][2U] - matrix[1U][2U] * matrix[2U][0U]) +
         matrix[0U][2U] * (matrix[1U][0U] * matrix[2U][1U] - matrix[1U][1U] * matrix[2U][0U]);
}

inline constexpr Mat3f RotationMatrix(float angle) {
  float cos = std::cos(angle);
  float sin = std::sin(angle);

  return {{cos, -sin, 0.0f,
           sin,  cos, 0.0f,
          0.0f, 0.0f, 1.0f}};
}

inline constexpr Mat3f ScaleMatrix(Vec2f scale) {
  return {{scale.x,    0.0f, 0.0f,
              0.0f, scale.y, 0.0f,
              0.0f,    0.0f, 1.0f}};
}

inline constexpr Mat3f TranslationMatrix(Vec2f translation) {
  return {{1.0f, 0.0f, translation.x,
           0.0f, 1.0f, translation.y,
           0.0f, 0.0f,          1.0f}};
}

inline constexpr Mat3f OrthographicProjectionMatrix(float left, float right, float bottom, float top) {
  RA_ASSERT(right > left, "Invalid projection matrix horizontal params (left = %f, right = %f)", left, right);
  RA_ASSERT(top > bottom, "Invalid projection matrix vertical params (bottom = %f, top = %f)", bottom, top);

  return {{2.0f / (right - left),                  0.0f, -(right + left) / (right - left),
                            0.0f, 2.0f / (top - bottom), -(top + bottom) / (top - bottom),
                            0.0f,                  0.0f,                             1.0f}};
}

inline constexpr Mat3f OrthographicProjectionMatrix(float horizontal, float vertical) {
  return OrthographicProjectionMatrix(-horizontal / 2.0f, horizontal / 2.0f, -vertical / 2.0f, vertical / 2.0f);
}

}  // namespace ra::math