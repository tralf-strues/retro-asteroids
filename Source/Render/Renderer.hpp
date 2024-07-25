/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Renderer.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Asset/FontAtlas.hpp>
#include <Math/Mat3.hpp>
#include <Render/Color.hpp>
#include <Render/Image.hpp>

namespace ra::render {

class Renderer {
 public:
  void BeginFrame(ImageView<Color> render_target);
  void EndFrame();

  void CmdSetViewInfo(math::Mat3f proj_view);

  void CmdClear(Color clear_color);
  void CmdDrawLine(const math::Vec2f& ms_from, const math::Vec2f& ms_to, const math::Mat3f& transform, Color color,
                   float thickness = 1.0f);

  void CmdDrawImage(ImageView<const Color> image_view, const math::Vec2f& ndc_pos);
  void CmdDrawText(std::string_view text, const math::Vec2f& ndc_pos, const asset::FontAtlas& font);

 private:
  inline constexpr math::Vec2f ConvertNDCToFramebuffer(const math::Vec2f& ndc) const {
    float half_width  = rt_.Extent().x / 2.0f;
    float half_height = rt_.Extent().y / 2.0f;
    return math::Vec2f(half_width + ndc.x * half_width,
                       rt_.Extent().y - static_cast<float>(half_height + ndc.y * half_height));
  }

  inline constexpr math::Vec2f ConvertFramebufferToNDC(const math::Vec2f& pixel) const {
    float half_width  = rt_.Extent().x / 2.0f;
    float half_height = rt_.Extent().y / 2.0f;
    return math::Vec2f((pixel.x - half_width) / half_width, (half_height - pixel.y) / half_height);
  }

  inline bool PixelCorrect(const math::Vec2i& pixel) const {
    return pixel.x >= 0 && pixel.x < static_cast<int32_t>(rt_.Extent().x) && pixel.y >= 0 &&
           pixel.y < static_cast<int32_t>(rt_.Extent().y);
  }

  inline void SetPixel(const math::Vec2i& pixel, Color color) {
    if (PixelCorrect(pixel)) {
      rt_(pixel.x, pixel.y) = color;
    }
  }

  inline Color GetPixel(const math::Vec2i& pixel) const {
    return PixelCorrect(pixel) ? rt_(pixel.x, pixel.y) : Color(0U);
  }

  inline void SetPixelBlended(const math::Vec2i& pixel, const math::Vec4f& normalized) {
    math::Vec4f old{GetPixel(pixel)};
    math::Vec4f result;

    result.rgb = normalized.rgb * normalized.a + old.rgb * (1.0f - normalized.a);
    result.a   = normalized.a + old.a * (1.0f - normalized.a);

    SetPixel(pixel, Color(result));
  }

  void CmdDrawImage(ImageView<const Color> image_view, const math::Vec2i& pos);

  ImageView<Color> rt_;
  math::Mat3f proj_view_;
};

}  // namespace ra::render