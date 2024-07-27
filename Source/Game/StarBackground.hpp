/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file StarBackground.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Render/Color.hpp>
#include <Render/ImageView.hpp>

namespace ra {

namespace job {
class Executor;
}  // namespace job

void RenderBackground(job::Executor& executor, render::ImageView<render::Color>& render_target, float time);

}  // namespace ra