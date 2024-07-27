/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file ImageLoader.cpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#include <Asset/ImageLoader.hpp>

#include <Utils/Assert.hpp>

#include <fstream>

namespace {

constexpr uint16_t kMagicNumberBMP = 0x4D42;

#pragma pack(push, 1)
struct FileHeaderBMP {
  uint16_t file_type{kMagicNumberBMP};
  uint32_t file_size{0U};
  uint16_t reserved1{0U};
  uint16_t reserved2{0U};
  uint32_t offset_data{0U};
};

struct InfoHeaderBMP {
  uint32_t size{0U};
  int32_t  width{0U};
  int32_t  height{0U};

  uint16_t planes{1U};
  uint16_t bit_count{0U};
  uint32_t compression{0U};
  uint32_t size_image{0U};
  int32_t  x_pixels_per_meter{0U};
  int32_t  y_pixels_per_meter{0U};
  uint32_t colors_used{0U};
  uint32_t colors_important{0U};
};

struct ColorHeaderBMP {
  uint32_t red_mask{0x00ff0000};
  uint32_t green_mask{0x0000ff00};
  uint32_t blue_mask{0x000000ff};
  uint32_t alpha_mask{0xff000000};
  uint32_t color_space_type{0x73524742};

  uint32_t unused[16U]{0U};
};
#pragma pack(pop)

}  // namespace

namespace ra::asset {

std::shared_ptr<render::Image<render::Color>> LoadImage_BMP(const std::filesystem::path& filepath) {
  RA_LOG_INFO("Loading image \"%s\"...", filepath.c_str());

  std::ifstream fs(filepath, std::ios_base::binary);
  if (!fs.is_open()) {
    RA_LOG_ERROR("Failed to open file \"%s\"", filepath.c_str());
    return nullptr;
  }

  FileHeaderBMP file_header;
  fs.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
  if (file_header.file_type != kMagicNumberBMP) {
    RA_LOG_ERROR("Invalid file header's type 0x%X (must be 0x%X)", static_cast<uint32_t>(file_header.file_type),
                 static_cast<uint32_t>(kMagicNumberBMP));
    return nullptr;
  }

  InfoHeaderBMP info_header;
  fs.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

  if (info_header.height < 0) {
    RA_LOG_ERROR("Currently parser does not handle negative image height (height=%d)", info_header.height);
    return nullptr;
  }

  if (info_header.bit_count != 32U) {
    RA_LOG_ERROR("Currently parser does not handle images not with 32-bit depth (depth=%u)",
                 static_cast<uint32_t>(info_header.bit_count));
    return nullptr;
  }

  fs.seekg(file_header.offset_data, std::ifstream::beg);

  const uint32_t row_size_bytes = info_header.width * (info_header.bit_count / 8U);
  const uint32_t size_bytes = row_size_bytes * static_cast<uint32_t>(info_header.height);

  auto pixels = std::make_unique<render::Color[]>(size_bytes / sizeof(render::Color));
  auto image  = std::make_shared<render::Image<render::Color>>(std::move(pixels),
                                                               math::Vec2u(info_header.width, info_header.height));

  for (int32_t y = 0; y < info_header.height; ++y) {
    auto target_row = image->CreateView().Row(info_header.height - y - 1);
    fs.read(reinterpret_cast<char*>(target_row.data()), row_size_bytes);
  }

  return image;
}

}  // namespace ra::asset