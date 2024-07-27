/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file PolygonLoader.cpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#include <Asset/PolygonLoader.hpp>

#include <fstream>

namespace ra::asset {

std::shared_ptr<render::Polygon> LoadPolygon(const std::filesystem::path& filepath) {
  RA_LOG_INFO("Loading polygon \"%s\"...", filepath.c_str());

  std::ifstream fs(filepath);
  if (!fs.is_open()) {
    RA_LOG_ERROR("Failed to open file \"%s\"", filepath.c_str());
    return nullptr;
  }

  auto polygon = std::make_shared<render::Polygon>();
  polygon->color     = render::Color(0xFFFFFFFF);
  polygon->thickness = 1;

  int32_t symmetry_start = -1;

  std::string line;
  int32_t     point_idx{0U};
  while (std::getline(fs, line)) {
    uint32_t rgba[4];
    float    thickness;
    size_t   pos_start;
    float    pos[2];

    if (line.find("symmetry") != std::string::npos) {
      symmetry_start = point_idx;
    } else if (std::sscanf(line.c_str(), "color=rgba(%u, %u, %u, %u)", &rgba[0], &rgba[1], &rgba[2], &rgba[3]) == 4) {
      polygon->color = render::Color(rgba[0], rgba[1], rgba[2], rgba[3]);
    } else if (std::sscanf(line.c_str(), "thickness=%f", &thickness) == 1) {
      polygon->thickness = thickness;
    } else if (pos_start = line.find("=vec2(");
               (pos_start != std::string::npos) &&
               (std::sscanf(line.substr(pos_start).c_str(), "=vec2(%f,%f)", &pos[0], &pos[1]) == 2)) {
      polygon->vertices.push_back({math::Vec2f(pos[0], pos[1]), false});
      ++point_idx;
    }
  }

  if (point_idx == 0) {
    RA_LOG_ERROR("No vertices found in \"%s\"", filepath.c_str());
    return nullptr;
  }

  if (symmetry_start != -1) {
    int32_t end_vertex = polygon->vertices.size();
    for (int32_t vertex = end_vertex - 1; vertex >= symmetry_start; --vertex) {
      polygon->vertices.push_back({
        .ms_position = math::Vec2f(-polygon->vertices[vertex].ms_position.x, polygon->vertices[vertex].ms_position.y),
        .split       = false
      });
    }
  }

  return polygon;
}

}  // namespace ra::asset