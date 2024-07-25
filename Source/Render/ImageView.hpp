/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file ImageView.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Math/Vec2.hpp>
#include <Utils/Assert.hpp>

#include <span>

namespace ra::render {

template <typename PixelType>
class ImageView {
 public:
  ImageView() = default;
  ImageView(PixelType* image_pixels, math::Vec2u image_extent, math::Vec2u region_offset, math::Vec2u region_extent);

  template <typename OtherPixelType>
  requires (sizeof(PixelType) == sizeof(OtherPixelType))
  ImageView(const ImageView<OtherPixelType>& other);

  std::span<PixelType> Row(uint32_t y);
  std::span<PixelType> Row(uint32_t y) const;

  PixelType& operator()(uint32_t x, uint32_t y);
  PixelType& operator()(uint32_t x, uint32_t y) const;

  const math::Vec2u& Offset() const;
  const math::Vec2u& Extent() const;

  std::span<PixelType> Data();
  std::span<PixelType> Data() const;

 private:
  PixelType*  image_pixels_{nullptr};
  math::Vec2u image_extent_{0U};

  math::Vec2u region_offset_{0U};
  math::Vec2u region_extent_{0U};

  template <typename OtherPixelType>
  friend class ImageView;
};

template <typename PixelType>
ImageView<PixelType>::ImageView(PixelType* image_pixels, math::Vec2u image_extent, math::Vec2u region_offset,
                                math::Vec2u region_extent)
    : image_pixels_(image_pixels),
      image_extent_(std::move(image_extent)),
      region_offset_(std::move(region_offset)),
      region_extent_(std::move(region_extent)) {}

template <typename PixelType>
template <typename OtherPixelType>
  requires(sizeof(PixelType) == sizeof(OtherPixelType))
ImageView<PixelType>::ImageView(const ImageView<OtherPixelType>& other)
    : image_pixels_(other.image_pixels_),
      image_extent_(other.image_extent_),
      region_offset_(other.region_offset_),
      region_extent_(other.region_extent_) {}

template <typename PixelType>
std::span<PixelType> ImageView<PixelType>::Row(uint32_t y) {
  return std::span(image_pixels_ + (region_offset_.y + y) * image_extent_.x + region_offset_.x, region_extent_.x);
}

template <typename PixelType>
std::span<PixelType> ImageView<PixelType>::Row(uint32_t y) const {
  return std::span(image_pixels_ + (region_offset_.y + y) * image_extent_.x + region_offset_.x, region_extent_.x);
}

template <typename PixelType>
PixelType& ImageView<PixelType>::operator()(uint32_t x, uint32_t y) {
  return Row(y)[x];
}

template <typename PixelType>
PixelType& ImageView<PixelType>::operator()(uint32_t x, uint32_t y) const {
  return Row(y)[x];
}

template <typename PixelType>
const math::Vec2u& ImageView<PixelType>::Offset() const {
  return region_offset_;
}

template <typename PixelType>
const math::Vec2u& ImageView<PixelType>::Extent() const {
  return region_extent_;
}

template <typename PixelType>
std::span<PixelType> ImageView<PixelType>::Data() {
  RA_ASSERT(region_offset_ == math::Vec2u(0U) && region_extent_ == image_extent_,
            "In order to access entire ImageView data, it must cover the entire underlying image");

  return std::span(image_pixels_, image_extent_.x * image_extent_.y);
}

template <typename PixelType>
std::span<PixelType> ImageView<PixelType>::Data() const {
  RA_ASSERT(region_offset_ == math::Vec2u(0U) && region_extent_ == image_extent_,
            "In order to access entire ImageView data, it must cover the entire underlying image");

  return std::span(image_pixels_, image_extent_.x * image_extent_.y);
}

}  // namespace ra::render