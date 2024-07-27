/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file StarBackground.cpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#include <Game/StarBackground.hpp>

#include <JobSystem/Executor.hpp>

namespace ra {

static inline float Random(math::Vec2f co) {
  float tmp;
  return std::modf(std::sin(math::Dot(co ,math::Vec2f(12.9898f,78.233f))) * 43758.5453f, &tmp);
}

static inline math::Vec4f PixelShader(math::Vec2f coords, math::Vec2f resolution, float time) {
  float size = 4.0f;
  float prob = 0.998f;

  math::Vec2f pos = 1.0f / size * coords;
  pos = math::Vec2f(std::floor(pos.x), std::floor(pos.y));

  float color      = 0.0f;
  float star_value = Random(pos);

  if (star_value > prob) {
    math::Vec2f center = size * pos + math::Vec2f(size, size) * 0.5f;

    float t = 0.9f + 0.2f * std::sin(time + (star_value - prob) / (1.0f - prob) * 45.0f);

    color = 1.0f - math::Length(coords - center) / (0.5f * size);
    color = color * t / (std::abs(coords.y - center.y)) * t / (std::abs(coords.x - center.x));
  } else if (Random(coords / resolution) > 0.996f) {
    float r = Random(coords);
    color   = r * (0.25f * std::sin(time * (r * 5.0f) + 720.0f * r) + 0.75f);
  }

  return math::Vec4f(color);
}

void RenderBackground(job::Executor& executor, render::ImageView<render::Color>& render_target, float time) {
  const auto extent  = static_cast<math::Vec2u>(render_target.Extent());
  const auto extentf = static_cast<math::Vec2f>(extent);

  auto  work_group_count = executor.ThreadCount();
  auto  work_group_size  = (extent.y + executor.ThreadCount() - 1U) / executor.ThreadCount();

  for (uint32_t work_group = 0U; work_group < work_group_count; ++work_group) {
    executor.Submit([=]() {
      for (uint32_t y = 0U; y < work_group_size; ++y) {
        const uint32_t final_y = work_group * work_group_size + y;

        for (uint32_t x = 0U; x < extent.x; ++x) {
          render_target(x, final_y) = render::Color(PixelShader(math::Vec2f(x, final_y), extentf, time));
        }
      }
    });
  }
}

}  // namespace ra