/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file ParticleSystem.hpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Render/Polygon.hpp>

namespace ra::render {

class Renderer;

class ParticleSystem {
 public:
  struct ParticleSpecs {
    math::Vec2f origin;
    math::Vec2f velocity;
    math::Vec2f velocity_variation;

    math::Vec4f color_begin;
    math::Vec4f color_end;

    float size_begin{0.0f};
    float size_end{1.0f};
    float size_variation{0.0f};

    float lifetime{1.0f};
  };

  ParticleSystem() = default;
  explicit ParticleSystem(size_t pool_size);

  void Update(float dt);
  void Render(render::Renderer& renderer);

  void EmitParticle(const ParticleSpecs& particleSpecs);

 private:
  struct Particle {
    math::Vec2f translation;
    math::Vec2f velocity;
    float       rotation{0.0f};
    math::Vec4f color_begin;
    math::Vec4f color_end;

    float size_begin{0.0f};
    float size_end{1.0f};

    float lifetime{1.0f};
    float time_remaining{0.0f};

    bool active{false};
  };

  static constexpr float kParticleRotationRate = 0.02f;

  Polygon               polygon_;
  std::vector<Particle> particles_;
  uint32_t              next_particle_{0U};
};

}  // namespace ra::render