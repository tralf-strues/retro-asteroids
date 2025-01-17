/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file ImageLoader.hpp
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Render/Color.hpp>
#include <Render/Image.hpp>

#include <filesystem>
#include <memory>

namespace ra::asset {

std::shared_ptr<render::Image<render::Color>> LoadImage_BMP(const std::filesystem::path& filepath);

}  // namespace ra::asset