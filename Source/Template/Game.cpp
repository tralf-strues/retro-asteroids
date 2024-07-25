#include "Engine.h"
#include <stdlib.h>
#include <memory.h>

#include <stdio.h>

#include <Asset/ImageLoader.hpp>
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

// initialize game data in this function
void initialize()
{
  RA_LOG_INFO("This is info message %u", 4U);
  RA_LOG_WARN("This is warn message %u", 1U);
  RA_LOG_ERROR("This is error message %u", 2U);
  RA_LOG_FATAL("This is fatal message %u", 3U);
  RA_LOG_INFO("This is an argument-less message");
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

  renderer.CmdDrawLine(ra::math::Vec2f(0.0f, 0.1f),
                       ra::math::Vec2f(0.8f, 0.4f),
                       ra::math::Identity<float>(),
                       ra::render::Color(0xFFFFFFFF), 3.0f);

  renderer.CmdDrawImage(image.CreateView(), ra::math::Vec2f(0.0f, 0.0f));
  renderer.CmdDrawText("Hello, World.", ra::math::Vec2f(-0.5f, -0.5f), font);

  renderer.EndFrame();
}

// free game data in this function
void finalize()
{
}
