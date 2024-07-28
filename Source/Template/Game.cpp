#include "Engine.h"
#include <stdlib.h>
#include <memory.h>

#include <stdio.h>

#include <Game/Game.hpp>

//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, VK_RETURN)
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  schedule_quit_game() - quit game after act()

std::unique_ptr<ra::Game> g_game{nullptr};

ra::render::ImageView<ra::render::Color> g_render_target{
  reinterpret_cast<ra::render::Color*>(buffer),
  ra::math::Vec2u(SCREEN_WIDTH, SCREEN_HEIGHT),
  ra::math::Vec2u(0U),
  ra::math::Vec2u(SCREEN_WIDTH, SCREEN_HEIGHT)
};

// initialize game data in this function
void initialize()
{
  g_game = std::make_unique<ra::Game>();
  g_game->StartNew();
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();

  g_game->Update(dt);

  static uint32_t fif = 0U;
  if (fif >= 60U) {
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
  g_game->Render(g_render_target);
}

// free game data in this function
void finalize()
{
  g_game.reset();
}
