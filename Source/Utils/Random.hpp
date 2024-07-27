/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Random.hpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec2.hpp>

#include <random>

namespace ra::utils {

class Random {
 public:
  static Random& Instance();

  float Normalized();
  float InRange(float start, float end);
  math::Vec2f InRange(const math::Vec2f& start, const math::Vec2f& end);

 private:
  Random();

  std::random_device device_;
  std::mt19937       generator_;

  static Random instance_;
};

}  // namespace ra::utils