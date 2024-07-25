/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file FontAtlas.hpp
 * @date 2024-07-22
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Render/Color.hpp>
#include <Render/Image.hpp>

#include <filesystem>
#include <optional>
#include <unordered_map>

namespace ra::asset {

struct FontAtlas {
  struct CharacterInfo {
    math::Vec2i position_in_image;
    math::Vec2i extent_pixels;
    math::Vec2i offset;
    int32_t     advance;
  };

  using Image            = ::ra::render::Image<render::Color>;
  using CharacterInfoMap = std::unordered_map<char, CharacterInfo>;
  using ImageViewMap     = std::unordered_map<char, render::ImageView<render::Color>>;

  Image            image;
  ImageViewMap     image_views;
  CharacterInfoMap characters;

  math::Vec4i padding_urdl;
  math::Vec2i spacing;
  int32_t     line_height;
  int32_t     base_y;
};

std::optional<FontAtlas> LoadFontAtlas_BMFontAtlas(const std::filesystem::path& image_filepath,
                                                   const std::filesystem::path& font_filepath);

}  // namespace ra::asset
