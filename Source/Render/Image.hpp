/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Image.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Render/ImageView.hpp>

namespace ra::render {

template <typename PixelType>
class Image {
 public:
  using PixelData = std::unique_ptr<PixelType[]>;
  using ImageView = ImageView<PixelType>;

  static constexpr math::Vec2u kWholeExtent = math::Vec2u(std::numeric_limits<uint32_t>::max());

  Image() = default;
  Image(PixelData&& pixels, const math::Vec2u& extent);

  [[nodiscard]] ImageView CreateView(math::Vec2u offset = math::Vec2u(0U), math::Vec2u extent = kWholeExtent);
  [[nodiscard]] ImageView CreateView(math::Vec2u offset = math::Vec2u(0U), math::Vec2u extent = kWholeExtent) const;

  [[nodiscard]] const math::Vec2u& Extent() const;

 private:
  PixelData   pixels_{nullptr};
  math::Vec2u extent_{0U};
};

template <typename PixelType>
Image<PixelType>::Image(PixelData&& pixels, const math::Vec2u& extent) : pixels_(std::move(pixels)), extent_(extent) {}

template <typename PixelType>
Image<PixelType>::ImageView Image<PixelType>::CreateView(math::Vec2u offset, math::Vec2u extent) {
  return ImageView(pixels_.get(), extent_, std::move(offset), extent == kWholeExtent ? extent_ : std::move(extent));
}

template <typename PixelType>
Image<PixelType>::ImageView Image<PixelType>::CreateView(math::Vec2u offset, math::Vec2u extent) const {
  return ImageView(pixels_.get(), extent_, std::move(offset), extent == kWholeExtent ? extent_ : std::move(extent));
}

template <typename PixelType>
const math::Vec2u& Image<PixelType>::Extent() const {
  return extent_;
}

}  // namespace ra::render