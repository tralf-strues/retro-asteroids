/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file UpdateSystems.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Game/Components.hpp>

#include <span>

namespace ra::systems {

void CalculateTransforms(std::span<const Transform> transforms, std::span<TransformMatrix> matrices) {
  const auto size = transforms.size();

  for (auto i = 0U; i < size; ++i) {
    auto translation_matrix = math::TranslationMatrix(transforms[i].pos);
    auto rotation_matrix    = math::RotationMatrix(transforms[i].rotation);
    auto scale_matrix       = math::ScaleMatrix(math::Vec2f(transforms[i].scale));

    matrices[i].matrix = rotation_matrix * scale_matrix;
    matrices[i].matrix = translation_matrix * matrices[i].matrix;
  }
}

}  // namespace ra::systems