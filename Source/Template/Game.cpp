#include "Engine.h"
#include <stdlib.h>
#include <memory.h>

#include <stdio.h>

#include <Asset/ImageLoader.hpp>
#include <ECS/World.hpp>
#include <JobSystem/Executor.hpp>
#include <Render/Renderer.hpp>
#include <Utils/Assert.hpp>

//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, VK_RETURN)
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  schedule_quit_game() - quit game after act()

ra::render::Image image = ra::asset::LoadImage_BMP("Assets/test_image.bmp").value();

ra::asset::FontAtlas font =
    ra::asset::LoadFontAtlas_BMFontAtlas("Assets/font_48/font_48.bmp", "Assets/font_48/font_48.fnt").value();

ra::ecs::World world;

ra::job::Executor job_executor;

struct Position {
  ra::math::Vec2f pos{10.0f};
};

struct Velocity {
  float vel{22.0f};
};

struct Health {
  int32_t hp{100};
};

void SimpleSystem(std::span<Position> positions, std::span<const Velocity> velocities) {
  for (auto idx = 0U; idx < positions.size(); ++idx) {
    positions[idx].pos += ra::math::Vec2f(velocities[idx].vel) * 0.01f;

    RA_LOG_INFO("pos = (%f, %f)", positions[idx].pos.x, positions[idx].pos.y);
    RA_LOG_INFO("vel = %f",       velocities[idx].vel);
  }

  std::printf("\n");
}

// initialize game data in this function
void initialize()
{
  RA_LOG_INFO("This is info message %u", 4U);
  RA_LOG_WARN("This is warn message %u", 1U);
  RA_LOG_ERROR("This is error message %u", 2U);
  RA_LOG_FATAL("This is fatal message %u", 3U);
  RA_LOG_INFO("This is an argument-less message");

  /* First entity */
  auto first = world.NewEntity();
  world.Add<Position>(first);

  RA_LOG_INFO("first.pos = (%f, %f)\n", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);

  world.Add<Velocity>(first);

  RA_LOG_INFO("first.pos = (%f, %f)", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);
  RA_LOG_INFO("first.vel = %f\n", world.Get<Velocity>(first).vel);

  world.Get<Position>(first).pos.x = 22.0f;
  world.Get<Position>(first).pos.y = -22.0f;
  world.Get<Velocity>(first).vel   = 10.0f;

  RA_LOG_INFO("first.pos = (%f, %f)", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);
  RA_LOG_INFO("first.vel = %f\n", world.Get<Velocity>(first).vel);

  /* Second entity */
  auto second = world.NewEntity();

  RA_LOG_INFO("first.pos = (%f, %f)", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);
  RA_LOG_INFO("first.vel = %f\n", world.Get<Velocity>(first).vel);

  /* Third entity */
  auto third = world.NewEntity();
  world.Add<Health>(third);

  RA_LOG_INFO("first.pos = (%f, %f)", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);
  RA_LOG_INFO("first.vel = %f", world.Get<Velocity>(first).vel);
  RA_LOG_INFO("third.hp  = %d\n", world.Get<Health>(third).hp);

  world.Add<Velocity>(third);

  world.Run(&SimpleSystem);

  RA_LOG_INFO("first.pos = (%f, %f)", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);
  RA_LOG_INFO("first.vel = %f", world.Get<Velocity>(first).vel);
  RA_LOG_INFO("third.hp  = %d", world.Get<Health>(third).hp);
  RA_LOG_INFO("third.vel = %f\n", world.Get<Velocity>(third).vel);

  world.Remove<Velocity>(first);
  world.DestroyEntity(second);

  RA_LOG_INFO("first.pos = (%f, %f)", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);
  RA_LOG_INFO("third.hp  = %d", world.Get<Health>(third).hp);
  RA_LOG_INFO("third.vel = %f\n", world.Get<Velocity>(third).vel);

  world.Add<Health>(first);
  world.Remove<Health>(third);

  RA_LOG_INFO("first.pos = (%f, %f)", world.Get<Position>(first).pos.x, world.Get<Position>(first).pos.y);
  RA_LOG_INFO("first.hp  = %d", world.Get<Health>(first).hp);
  RA_LOG_INFO("third.vel = %f\n", world.Get<Velocity>(third).vel);

  world.DestroyEntity(first);

  RA_LOG_INFO("third.vel = %f\n", world.Get<Velocity>(third).vel);

  world.DestroyEntity(third);
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();

  static uint32_t fif = 0U;
  if (fif >= 30U) {
    RA_LOG_INFO("Frame time is %.2f ms", dt * 1e3);

    fif = 0U;
  } else {
    ++fif;
  }
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
  ra::render::Renderer renderer;

  renderer.BeginFrame(ra::render::ImageView {
    reinterpret_cast<ra::render::Color*>(buffer),
    ra::math::Vec2u(SCREEN_WIDTH, SCREEN_HEIGHT),
    ra::math::Vec2u(0U),
    ra::math::Vec2u(SCREEN_WIDTH, SCREEN_HEIGHT)
  });

  renderer.CmdSetViewInfo(ra::math::OrthographicProjectionMatrix(SCREEN_WIDTH / 100.0f, SCREEN_HEIGHT / 100.0f));

    renderer.CmdClear(ra::render::Color(0x33, 0x44, 0x55)); 

  job_executor.Submit([&]() {
    renderer.CmdDrawLine(ra::math::Vec2f(0.0f, 0.1f),
                        ra::math::Vec2f(0.8f, 0.4f),
                        ra::math::Identity<float>(),
                        ra::render::Color(0xFFFFFFFF), 3.0f);
  });

  job_executor.Submit([&]() {
    renderer.CmdDrawImage(image.CreateView(), ra::math::Vec2f(0.0f, 0.0f));
  });

  job_executor.Submit([&]() {
    renderer.CmdDrawText("Hello, World.", ra::math::Vec2f(-0.5f, -0.5f), font);
  });

  job_executor.WaitIdle();

  renderer.EndFrame();
}

// free game data in this function
void finalize()
{
  job_executor.Stop();
}
