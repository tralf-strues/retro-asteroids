/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Random.cpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#include <Utils/Random.hpp>

namespace ra::utils {

Random Random::instance_;

Random& Random::Instance() { return instance_; }

Random::Random() : generator_(device_()) {}

float Random::Normalized() {
  return InRange(0.0f, 1.0f);
}

float Random::InRange(float start, float end) {
  std::uniform_real_distribution<float> dist(start, end);
  return dist(generator_);
}

math::Vec2f Random::InRange(const math::Vec2f& start, const math::Vec2f& end) {
  return math::Vec2f(InRange(start.x, end.x), InRange(start.y, end.y));
}

}  // namespace ra::utils