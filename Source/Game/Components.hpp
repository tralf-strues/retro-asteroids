/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Components.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Mat3.hpp>
#include <Render/Polygon.hpp>

#include <memory>

namespace ra {

/* World location components */
struct Transform {
  math::Vec2f pos{0.0f};
  float       rotation{0.0f};
  float       scale{1.0f};
};

struct TransformMatrix {
  math::Mat3f matrix;
};

/* Render components */
struct PolygonRenderer {
  std::shared_ptr<render::Polygon> polygon;
};

}  // namespace ra