/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Game.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <ECS/World.hpp>
#include <Game/DeferQueue.hpp>
#include <Game/StarBackground.hpp>
#include <JobSystem/Executor.hpp>
#include <Render/ParticleSystem.hpp>
#include <Render/Renderer.hpp>

namespace ra {

class Game {
 public:
  ~Game();

  void StartNew();

  void Update(float dt);
  void Render(render::ImageView<render::Color>& render_target);

 protected:
  void ProcessZoom();
  void RenderUI();

  void LoadHighScore();
  void StoreHighScore();

  job::Executor    executor_;
  render::Renderer renderer_;

  ecs::World world_;
  DeferQueue defer_queue_;

  double   time_{0.0};
  int32_t  zoom_{25};
  uint32_t score_{0U};
  uint32_t highest_score_{0U};
  bool     score_saved_{false};
  bool     game_over_{false};

  ecs::EntityId player_;
  int32_t       enemy_level_{0};
  int32_t       enemies_left_{0};

  ecs::EntityId explosions_;
  PrecalculatedStarsData stars_data_;
};

}  // namespace ra