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
#include <sstream>

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

static void ParseInfo(std::stringstream& ss, FontAtlas& font) {
  font.padding_urdl = ParseTokenVec4i<"padding">(ss.view());
  font.spacing      = ParseTokenVec2i<"spacing">(ss.view());
}

static void ParseCommon(std::stringstream& ss, FontAtlas& font) {
  font.line_height = ParseTokenInt<"lineHeight">(ss.view());
  font.base_y      = ParseTokenInt<"base">(ss.view());
}

static void ParseChar(std::stringstream& ss, FontAtlas& font) {
  char ch = ParseTokenInt<"id">(ss.view());

  FontAtlas::CharacterInfo info;
  info.position_in_image.x = ParseTokenInt<"x">(ss.view());
  info.position_in_image.y = ParseTokenInt<"y">(ss.view());
  info.extent_pixels.x     = ParseTokenInt<"width">(ss.view());
  info.extent_pixels.y     = ParseTokenInt<"height">(ss.view());
  info.offset.x            = ParseTokenInt<"xoffset">(ss.view());
  info.offset.y            = ParseTokenInt<"yoffset">(ss.view());
  info.advance             = ParseTokenInt<"xadvance">(ss.view());

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

    std::stringstream ss(std::move(line));

    std::string tag;
    ss >> tag;
    if (tag == "info") {
      ParseInfo(ss, font);
    } else if (tag == "common") {
      ParseCommon(ss, font);
    } else if (tag == "char") {
      ParseChar(ss, font);
    }
  }

  return font;
}

}  // namespace ra::asset
