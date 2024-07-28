/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file StarBackground.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Render/Color.hpp>
#include <Render/Image.hpp>
#include <Render/ImageView.hpp>

namespace ra {

namespace job {
class Executor;
}  // namespace job

struct PrecalculatedStarsData {
  render::Image<math::Vec4f> image_dist1;  // x = distr, yz = pos, w = star_value
  render::Image<float>       image_dist2;

  render::ImageView<math::Vec4f> view_dist1;
  render::ImageView<float>       view_dist2;
};

PrecalculatedStarsData PrecalculateStarPositions(job::Executor& executor, math::Vec2u extent);

void RenderBackground(job::Executor& executor, render::ImageView<render::Color>& render_target,
                      const PrecalculatedStarsData& stars_data, float time);

}  // namespace ra