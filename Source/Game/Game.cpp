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

#include <fstream>
#include <sstream>

namespace ra {

static std::shared_ptr<asset::FontAtlas> g_font_atlas =
    asset::LoadFontAtlas_BMFontAtlas("Assets/font_48/font_48.bmp", "Assets/font_48/font_48.fnt");

Game::~Game() {
  executor_.Stop();
}

void Game::StartNew() {
  world_     = ecs::World{};
  game_over_ = false;
  score_     = 0U;
  score_saved_  = false;

  LoadHighScore();

  explosions_ = world_.NewEntity();
  world_.Add<render::ParticleSystem>(explosions_) = render::ParticleSystem(2048U);
  world_.Add<render::ParticleSystem::ParticleSpecs>(explosions_) = {
    .origin             = math::Vec2f(0.0f),
    .velocity           = math::Vec2f(0.0f),
    .velocity_variation = math::Vec2f(100.0f),

    .color_begin = math::Vec4f(0.9f, 0.8f, 0.1f, 1.0f),
    .color_end   = math::Vec4f(1.0f, 0.4f, 0.1f, 0.0f),

    .size_begin     = 0.05f,
    .size_end       = 0.3f,
    .size_variation = 0.05f,

    .lifetime = 0.1f
  };

  player_       = SpawnPlayer(world_);
  enemy_level_  = 0U;
  enemies_left_ = 0U;
}

void Game::Update(float dt) {
  time_ += dt;

  if (game_over_ && input::CheckKey(input::Key::Enter)) {
      StartNew();
  } else if (game_over_ && !score_saved_) {
    StoreHighScore();
    return;
  } else if (game_over_) {
    return;
  }

  ProcessZoom();

  /* Systems */
  world_.Run(renderer_, &systems::ProcessInput);
  world_.Run(dt, &systems::Move);
  world_.Run(world_, &systems::Follow);
  world_.Run(&systems::CalculateTransforms);
  world_.Run(&systems::UpdateColliders);

  systems::ContextShooting context_shooting{.defer_queue = defer_queue_, .dt = dt};
  world_.Run(context_shooting, &systems::Shoot);

  world_.Run(dt, &systems::EmitPlayerParticles);
  world_.Run(dt, &systems::EmitUFOParticles);
  world_.Run(dt, &systems::UpdateParticles);

  systems::ContextCollisionDetection context_collision {
    .world           = world_,
    .defer_queue     = defer_queue_,
    .score           = score_,
    .enemies_left    = enemies_left_,
    .game_over       = game_over_,
    .explosions      = world_.Get<render::ParticleSystem>(explosions_),
    .explosion_specs = world_.Get<render::ParticleSystem::ParticleSpecs>(explosions_)
  };
  world_.template RunInteractions<systems::ContextCollisionDetection, SphereCollider>(context_collision,
                                                                                      &systems::CollisionDetection);

  systems::ContextSpawnNewEnemies context_spawn{
    .defer_queue  = defer_queue_,
    .target       = player_,
    .enemy_level  = enemy_level_,
    .enemies_left = enemies_left_
  };
  world_.Run(context_spawn, &systems::SpawnNewEnemies);

  world_.Run(defer_queue_, &systems::DestroyOnFarAway);

  executor_.WaitIdle();
}

void Game::Render(render::ImageView<render::Color>& render_target) {
  if (stars_data_.image_dist1.Extent() != render_target.Extent()) {
    stars_data_ = PrecalculateStarPositions(executor_, render_target.Extent());
    executor_.WaitIdle();
  }

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
  RenderBackground(executor_, render_target, stars_data_, static_cast<float>(time_));
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
  executor_.WaitIdle();

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
  if (game_over_) {
    renderer_.CmdDrawText("Game Over!", math::Vec2f(-0.3f, 0.4f), *g_font_atlas);

    std::stringstream ss;

    ss << "Score: " << score_;
    renderer_.CmdDrawText(ss.view(), math::Vec2f(-0.3f, 0.0f), *g_font_atlas);
    ss = {};

    ss << "Highest: " << highest_score_;
    renderer_.CmdDrawText(ss.view(), math::Vec2f(-0.3f, -0.2f), *g_font_atlas);
    ss = {};

    double tmp;
    auto blink = std::modf(2.0f * time_, &tmp);
    blink = math::Lerp(0.5f, 1.0f, blink);

    ss << "Press Enter";
    renderer_.CmdDrawText(ss.view(), math::Vec2f(-0.3f, -0.6f), *g_font_atlas, blink);
  } else {
    std::stringstream ss;
    ss << "SCORE: " << score_;

    renderer_.CmdDrawText(ss.view(), math::Vec2f(-0.95f, 0.95f), *g_font_atlas);
  }
}

static constexpr std::string_view kScoreFilepath = "highest_score.txt";

void Game::LoadHighScore() {
  RA_LOG_INFO("Loading highest score to \"%s\"...", kScoreFilepath.data());

  std::ifstream fs(kScoreFilepath.data());
  if (!fs.is_open()) {
    RA_LOG_WARN("No known highest score file found \"%s\"", kScoreFilepath.data());
    highest_score_ = 0U;
    return;
  }

  fs >> highest_score_;
}

void Game::StoreHighScore() {
  if (score_ <= highest_score_) {
    return;
  }

  highest_score_ = score_;

  RA_LOG_INFO("Saving highest score to \"%s\"...", kScoreFilepath.data());

  std::ofstream fs(kScoreFilepath.data());
  if (!fs.is_open()) {
    RA_LOG_ERROR("Failed to open file \"%s\"", kScoreFilepath.data());
    return;
  }

  fs << highest_score_;

  score_saved_ = true;
}

}  // namespace ra