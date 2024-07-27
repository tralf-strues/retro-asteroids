/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Polygon.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec2.hpp>
#include <Render/Color.hpp>

#include <vector>

namespace ra::render {

struct Polygon {
  struct Vertex {
    math::Vec2f ms_position;
    bool        split;
  };

  std::vector<Vertex> vertices;
  Color               color;
  float               thickness;
};

}  // namespace ra::render
