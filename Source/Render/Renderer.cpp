/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Renderer.cpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#include <Render/Renderer.hpp>

#include <Utils/Assert.hpp>

namespace ra::render {

/**
 * Modified version of https://github.com/miloyip/line SDF with AABB algorithm.
 */
static constexpr inline float CapsuleSDF(math::Vec2f pixel, math::Vec2f from, math::Vec2f to, float thickness) {
  if (to == from) {
    return 0;
  }

  auto from_to_pixel = pixel - from;
  auto line          = to - from;

  auto h     = std::max(std::min(math::Dot(from_to_pixel, line) / math::LengthSquared(line), 1.0f), 0.0f);
  auto delta = from_to_pixel - line * h;

  return math::Length(delta) - thickness;
}

void Renderer::BeginFrame(ImageView<Color> render_target) {
  rt_            = render_target;
}

void Renderer::EndFrame() {
}

void Renderer::CmdSetViewInfo(math::Mat3f proj_view, math::Mat3f inv_proj_view) {
  proj_view_     = std::move(proj_view);
  inv_proj_view_ = std::move(inv_proj_view);
}

void Renderer::CmdClear(Color clear_color) {
  if (rt_.Offset() == math::Vec2u(0U)) {
    auto data = rt_.Data();
    std::fill(data.begin(), data.end(), clear_color);
    return;
  }

  for (uint32_t y = 0U; y < rt_.Extent().y; ++y) {
    auto row = rt_.Row(y);
    std::fill(row.begin(), row.end(), clear_color);
  }
}

void Renderer::CmdDrawLine(const math::Vec2f& ms_from, const math::Vec2f& ms_to, const math::Mat3f& transform,
                           Color color, float thickness) {
  auto from = ConvertNDCToFramebuffer(math::Vec2f(proj_view_ * transform * math::Vec3f(ms_from, 1.0f)));
  auto to   = ConvertNDCToFramebuffer(math::Vec2f(proj_view_ * transform * math::Vec3f(ms_to, 1.0f)));

  auto x0 = static_cast<int32_t>(std::floor(std::min(from.x, to.x) - thickness));
  auto x1 = static_cast<int32_t>(std::ceil(std::max(from.x, to.x) + thickness));

  auto y0 = static_cast<int32_t>(std::floor(std::min(from.y, to.y) - thickness));
  auto y1 = static_cast<int32_t>(std::ceil(std::max(from.y, to.y) + thickness));

  math::Vec4f colorf(color);

  for (int32_t y = y0; y <= y1; ++y) {
    for (int32_t x = x0; x <= x1; ++x) {
      float alpha = std::max(std::min(0.5f - CapsuleSDF(math::Vec2f(x, y), from, to, thickness), 1.0f), 0.0f);
      SetPixelBlended(math::Vec2i(x, y), math::Vec4f(colorf.rgb, colorf.a * alpha));
    }
  }
}

void Renderer::CmdDrawPolygon(const Polygon& polygon, const math::Mat3f& transform) {
  size_t vertices_count = polygon.vertices.size();

  for (uint32_t vertex = 0U; vertex < vertices_count; ++vertex) {
    if (polygon.vertices[vertex].split || polygon.vertices[(vertex + 1U) % vertices_count].split) {
      continue;
    }

    CmdDrawLine(polygon.vertices[vertex].ms_position, polygon.vertices[(vertex + 1U) % vertices_count].ms_position,
                transform, polygon.color, polygon.thickness);
  }
}

void Renderer::CmdDrawImage(ImageView<Color const> view, const math::Vec2f& ndc_pos, float transparency) {
  CmdDrawImage(view, math::Vec2i(ConvertNDCToFramebuffer(ndc_pos)));
}

void Renderer::CmdDrawText(std::string_view text, const math::Vec2f& ndc_pos, const asset::FontAtlas& font,
                           float transparency) {
  auto cur_pos = math::Vec2i(ConvertNDCToFramebuffer(ndc_pos)) + math::Vec2i(font.padding_urdl.x, font.padding_urdl.w);

  for (char ch : text) {
    const auto& ch_info = font.characters.at(ch);
    const auto& view    = font.image_views.at(ch);

    CmdDrawImage(view, cur_pos + ch_info.offset, transparency);
    cur_pos.x += font.spacing.x + ch_info.advance;
  }
}

math::Vec2f Renderer::ScreenSpaceToWorld(const math::Vec2u& ss_pos) const {
  return math::Vec2f(inv_proj_view_ * math::Vec3f(ConvertFramebufferToNDC(math::Vec2f(ss_pos)), 1.0f));
}

void Renderer::CmdDrawImage(ImageView<const Color> view, const math::Vec2i& pos, float transparency) {
  for (uint32_t y = 0U; y < view.Extent().y; ++y) {
    for (uint32_t x = 0U; x < view.Extent().x; ++x) {
      auto color = math::Vec4f(view(x, y));
      color.a *= transparency;

      SetPixelBlended(pos + math::Vec2i(x, y), color);
    }
  }
}

}  // namespace ra::render
