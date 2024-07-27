/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Game.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <ECS/World.hpp>
#include <JobSystem/Executor.hpp>
#include <Render/Renderer.hpp>

namespace ra {

class Game {
 public:
  ~Game();

  void Start();

  void Update(float dt);
  void Render(render::ImageView<render::Color>& render_target);

 protected:
  job::Executor    executor_;
  render::Renderer renderer_;

  ecs::World world_;

  double  time_{0.0};
  int32_t zoom_{25};
};

}  // namespace ra