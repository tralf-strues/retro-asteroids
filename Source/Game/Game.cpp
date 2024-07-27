/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Game.cpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#include <Game/Game.hpp>

#include <Asset/PolygonLoader.hpp>
#include <Game/RenderSystems.hpp>
#include <Game/StarBackground.hpp>
#include <Game/UpdateSystems.hpp>

namespace ra {

Game::~Game() {
  executor_.Stop();
}

void Game::Start() {
  auto player = world_.NewEntity();
  world_.Add<Transform>(player);
  world_.Add<TransformMatrix>(player);
  auto& player_polygon = world_.Add<PolygonRenderer>(player);
  player_polygon.polygon = asset::LoadPolygon("Assets/Polygons/player_spaceship.txt");
}

void Game::Update(float dt) {
  time_ += dt;

  /* Systems */
  world_.Run(&systems::CalculateTransforms);

  executor_.WaitIdle();
}

void Game::Render(render::ImageView<render::Color>& render_target) {
  renderer_.BeginFrame(render_target);
  renderer_.CmdSetViewInfo(ra::math::OrthographicProjectionMatrix(
      render_target.Extent().x / static_cast<float>(zoom_),
      render_target.Extent().y / static_cast<float>(zoom_)
  ));

  RenderBackground(executor_, render_target, static_cast<float>(time_));
  executor_.WaitIdle();

  /* Systems */
  systems::ContextRenderPolygons context_polygons {
    .renderer = renderer_,
    .executor = executor_
  };
  world_.Run(context_polygons, &systems::RenderPolygons);

  executor_.WaitIdle();
  renderer_.EndFrame();
}

}  // namespace ra