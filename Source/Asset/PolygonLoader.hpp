/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file PolygonLoader.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Render/Polygon.hpp>

#include <filesystem>
#include <memory>

namespace ra::asset {

std::shared_ptr<render::Polygon> LoadPolygon(const std::filesystem::path& filepath);

}  // namespace ra::asset