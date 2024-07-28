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

static constexpr float kSize = 4.0f;
static constexpr float kProb = 0.998f;

static inline float Random(math::Vec2f co) {
  float tmp;
  return std::modf(std::sin(math::Dot(co ,math::Vec2f(12.9898f,78.233f))) * 43758.5453f, &tmp);
}

PrecalculatedStarsData PrecalculateStarPositions(job::Executor& executor, math::Vec2u extent) {
  PrecalculatedStarsData stars_data;

  stars_data.image_dist1 = render::Image<math::Vec4f>(extent);
  stars_data.image_dist2 = render::Image<float>(extent);
  stars_data.view_dist1  = stars_data.image_dist1.CreateView();
  stars_data.view_dist2  = stars_data.image_dist2.CreateView();

  auto  work_group_count = executor.ThreadCount();
  auto  work_group_size  = (extent.y + executor.ThreadCount() - 1U) / executor.ThreadCount();

  for (uint32_t work_group = 0U; work_group < work_group_count; ++work_group) {
    executor.Submit([&, work_group]() {
      for (uint32_t y = 0U; y < work_group_size; ++y) {
        const uint32_t final_y = work_group * work_group_size + y;

        for (uint32_t x = 0U; x < extent.x; ++x) {
          auto fcoords = math::Vec2f(x, final_y);

          math::Vec4f dist1;
          dist1.x = Random(fcoords);

          {
            auto pos = 1.0f / kSize * fcoords;
            pos      = math::Vec2f(std::floor(pos.x), std::floor(pos.y));
            dist1.y  = pos.x;
            dist1.z  = pos.y;
            dist1.w  = Random(pos);
          }

          stars_data.view_dist1(x, final_y) = dist1;
          stars_data.view_dist2(x, final_y) = Random(fcoords / math::Vec2f(extent));
        }
      }
    });
  }

  executor.WaitIdle();

  return stars_data;
}

static math::Vec4f PixelShader(const PrecalculatedStarsData& stars_data, math::Vec2u ucoords, float time) {
  auto coords = math::Vec2f(ucoords);
  auto color  = 0.0f;

  auto dist1      = stars_data.view_dist1(ucoords.x, ucoords.y);
  auto pos        = math::Vec2f(dist1.y, dist1.z);
  auto star_value = dist1.w;

  if (star_value > kProb) {
    math::Vec2f center = kSize * pos + math::Vec2f(kSize, kSize) * 0.5f;

    float t = 0.9f + 0.2f * std::sin(time + (star_value - kProb) / (1.0f - kProb) * 45.0f);

    color = 1.0f - math::Length(coords - center) / (0.5f * kSize);
    color = color * t / (std::abs(coords.y - center.y)) * t / (std::abs(coords.x - center.x));
  } else if (dist1.x > 0.996f) {
    float r = stars_data.view_dist2(ucoords.x, ucoords.y);
    color   = r * (0.25f * std::sin(time * (r * 5.0f) + 720.0f * r) + 0.75f);
  }

  return math::Vec4f(color, color, color, 1.0f);
}

void RenderBackground(job::Executor& executor, render::ImageView<render::Color>& render_target,
                      const PrecalculatedStarsData& stars_data, float time) {
  const auto extent  = static_cast<math::Vec2u>(render_target.Extent());

  auto  work_group_count = executor.ThreadCount();
  auto  work_group_size  = (extent.y + executor.ThreadCount() - 1U) / executor.ThreadCount();

  for (uint32_t work_group = 0U; work_group < work_group_count; ++work_group) {
    executor.Submit([=, &stars_data]() {
      for (uint32_t y = 0U; y < work_group_size; ++y) {
        const uint32_t final_y = work_group * work_group_size + y;

        for (uint32_t x = 0U; x < extent.x; ++x) {
          render_target(x, final_y) = render::Color(PixelShader(stars_data, math::Vec2u(x, final_y), time));
        }
      }
    });
  }

  executor.WaitIdle();
}

}  // namespace ra