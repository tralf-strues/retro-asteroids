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

#include <sstream>

namespace ra {

static std::shared_ptr<asset::FontAtlas> g_font_atlas =
    asset::LoadFontAtlas_BMFontAtlas("Assets/font_48/font_48.bmp", "Assets/font_48/font_48.fnt");

Game::~Game() {
  executor_.Stop();
}

void Game::Start() {
  auto player = SpawnPlayer(world_);

  SpawnUFO(world_, math::Vec2f( 10.0f,   10.0f), player);
  SpawnUFO(world_, math::Vec2f(-10.0f, -10.0f), player);
}

void Game::Update(float dt) {
  time_ += dt;

  ProcessZoom();

  /* Systems */
  // systems::ContextProcessInput context_process_input{.renderer = renderer_, .defer_queue = defer_queue_, .dt = dt};
  world_.Run(renderer_, &systems::ProcessInput);
  world_.Run(dt, &systems::Move);
  world_.Run(world_, &systems::Follow);
  world_.Run(&systems::CalculateTransforms);

  systems::ContextShooting context_shooting{.defer_queue = defer_queue_, .dt = dt};
  world_.Run(context_shooting, &systems::Shoot);

  world_.Run(dt, &systems::EmitPlayerParticles);
  world_.Run(dt, &systems::EmitUFOParticles);
  world_.Run(dt, &systems::UpdateParticles);

  world_.Run(defer_queue_, &systems::DestroyOnFarAway);

  executor_.WaitIdle();
}

void Game::Render(render::ImageView<render::Color>& render_target) {
  renderer_.BeginFrame(render_target);

  const auto proj_view = ra::math::OrthographicProjectionMatrix(
    render_target.Extent().x / static_cast<float>(zoom_),
    render_target.Extent().y / static_cast<float>(zoom_)
  );

  const auto inv_proj_view = ra::math::InverseOrthographicProjectionMatrix(
    render_target.Extent().x / static_cast<float>(zoom_),
    render_target.Extent().y / static_cast<float>(zoom_)
  );

  renderer_.CmdSetViewInfo(proj_view, inv_proj_view);

  /* Background */
  RenderBackground(executor_, render_target, static_cast<float>(time_));
  executor_.WaitIdle();

  /* Systems */
  systems::ContextRenderPolygons context_polygons {
    .renderer = renderer_,
    .executor = executor_
  };
  world_.Run(context_polygons, &systems::RenderPolygons);

  systems::ContextRenderParticles context_particles {
    .renderer = renderer_,
    .executor = executor_
  };
  world_.Run(context_particles, &systems::RenderParticles);

  /* UI */
  RenderUI();

  executor_.WaitIdle();

  defer_queue_.Execute(world_);
  defer_queue_.Clear();

  renderer_.EndFrame();
}

void Game::ProcessZoom() {
  if (input::CheckMouseButton(input::MouseButton::WheelUp)) {
    zoom_ += 1;
  } else if (input::CheckMouseButton(input::MouseButton::WheelDown)) {
    zoom_ -= 1;
  }
}

void Game::RenderUI() {
  std::stringstream ss;
  ss << "SCORE: " << score_;

  renderer_.CmdDrawText(ss.view(), math::Vec2f(-0.95f, 0.95f), *g_font_atlas);
}

}  // namespace ra