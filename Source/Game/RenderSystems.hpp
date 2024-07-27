/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file RenderSystems.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Game/Components.hpp>
#include <JobSystem/Executor.hpp>
#include <Render/Renderer.hpp>

#include <span>

namespace ra::systems {

struct ContextRenderPolygons {
  render::Renderer& renderer;
  job::Executor&    executor;
};

void RenderPolygons(ContextRenderPolygons& context,
                    std::span<const TransformMatrix> matrices,
                    std::span<const PolygonRenderer> polygons) {
  const auto size = matrices.size();

  for (auto i = 0U; i < size; ++i) {
    context.executor.Submit([=]() {
      context.renderer.CmdDrawPolygon(*polygons[i].polygon, matrices[i].matrix);
    });
  }
}

}  // namespace ra::systems