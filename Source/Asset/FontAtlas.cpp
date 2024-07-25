/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file FontAtlas.cpp
 * @date 2024-07-22
 *
 * @copyright Copyright (c) 2024
 */

#include <Asset/FontAtlas.hpp>

#include <Asset/ImageLoader.hpp>

#include <fstream>

namespace ra::asset {

template <utils::StringLiteral ParamName>
static int32_t ParseTokenInt(std::string_view buffer) {
  auto idx = buffer.find(ParamName.data);
  RA_ASSERT(idx != std::string_view::npos, "Failed to find parameter '%s'", ParamName);

  int32_t value;
  std::sscanf(buffer.substr(idx + ParamName.Size()).data(), "=%d", &value);

  return value;
}

template <utils::StringLiteral ParamName>
static math::Vec2i ParseTokenVec2i(std::string_view buffer) {
  auto idx = buffer.find(ParamName.data);
  RA_ASSERT(idx != std::string_view::npos, "Failed to find parameter '%s'", ParamName);

  math::Vec2i value;
  std::sscanf(buffer.substr(idx + ParamName.Size()).data(), "=%d,%d", &value.x, &value.y);

  return value;
}

template <utils::StringLiteral ParamName>
static math::Vec4i ParseTokenVec4i(std::string_view buffer) {
  auto idx = buffer.find(ParamName.data);
  RA_ASSERT(idx != std::string_view::npos, "Failed to find parameter '%s'", ParamName);

  math::Vec4i value;
  std::sscanf(buffer.substr(idx + ParamName.Size()).data(), "=%d,%d,%d,%d", &value.x, &value.y, &value.z, &value.w);

  return value;
}

static void ParseInfo(std::string_view view, FontAtlas& font) {
  font.padding_urdl = ParseTokenVec4i<"padding">(view);
  font.spacing      = ParseTokenVec2i<"spacing">(view);
}

static void ParseCommon(std::string_view view, FontAtlas& font) {
  font.line_height = ParseTokenInt<"lineHeight">(view);
  font.base_y      = ParseTokenInt<"base">(view);
}

static void ParseChar(std::string_view view, FontAtlas& font) {
  char ch = ParseTokenInt<"id">(view);

  FontAtlas::CharacterInfo info;
  info.position_in_image.x = ParseTokenInt<"x">(view);
  info.position_in_image.y = ParseTokenInt<"y">(view);
  info.extent_pixels.x     = ParseTokenInt<"width">(view);
  info.extent_pixels.y     = ParseTokenInt<"height">(view);
  info.offset.x            = ParseTokenInt<"xoffset">(view);
  info.offset.y            = ParseTokenInt<"yoffset">(view);
  info.advance             = ParseTokenInt<"xadvance">(view);

  font.characters[ch] = info;
  font.image_views[ch] = font.image.CreateView(math::Vec2u(info.position_in_image), math::Vec2u(info.extent_pixels));
}

std::optional<FontAtlas> LoadFontAtlas_BMFontAtlas(const std::filesystem::path& image_filepath,
                                                   const std::filesystem::path& font_filepath) {
  auto image = LoadImage_BMP(image_filepath);
  if (!image) {
    return std::nullopt;
  }

  RA_LOG_INFO("Loading font file \"%s\"...", font_filepath.c_str());

  std::ifstream fs(font_filepath, std::ios::in);
  if (!fs.is_open()) {
    RA_LOG_ERROR("Failed to open file \"%s\"", font_filepath.c_str());
    return std::nullopt;
  }

  FontAtlas font;
  font.image = std::move(image.value());

  while (!fs.eof()) {
    std::string line;
    std::getline(fs, line);

    if (line.find("info ") == 0U) {
      ParseInfo(line, font);
    } else if (line.find("common ") == 0U) {
      ParseCommon(line, font);
    } else if (line.find("char ") == 0U) {
      ParseChar(line, font);
    }
  }

  return font;
}

}  // namespace ra::asset
