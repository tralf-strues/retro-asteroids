/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file ParticleSystem.cpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#include <Render/ParticleSystem.hpp>

#include <Asset/PolygonLoader.hpp>
#include <Math/Mat3.hpp>
#include <Render/Renderer.hpp>
#include <Utils/Random.hpp>

namespace ra::render {

static const auto kParticlePolygon = asset::LoadPolygon("Assets/Polygons/fire_particle.txt");

ParticleSystem::ParticleSystem(size_t pool_size) {
  particles_.resize(pool_size);
  polygon_ = *kParticlePolygon;
}

void ParticleSystem::Update(float dt) {
  for (auto& particle : particles_) {
    if (!particle.active) {
      continue;
    }

    if (particle.time_remaining <= 0.0f) {
      particle.active = false;
      continue;
    }

    particle.translation += particle.velocity * dt;
    particle.rotation += kParticleRotationRate * dt;
    particle.time_remaining -= dt;
  }
}

void ParticleSystem::Render(render::Renderer& renderer){
  for (auto& particle : particles_) {
    if (!particle.active) {
      continue;
    }

    float       lifetime_percentage = particle.time_remaining / particle.lifetime;
    math::Vec4f color               = math::Lerp(particle.color_end, particle.color_begin, lifetime_percentage);
    float       size                = math::Lerp(particle.size_end, particle.size_begin, lifetime_percentage);

    polygon_.color        = Color(color);
    math::Mat3f transform = math::TranslationMatrix(particle.translation) * math::RotationMatrix(particle.rotation) *
                            math::ScaleMatrix(math::Vec2f(size, size));

    renderer.CmdDrawPolygon(polygon_, transform);
  }
}

void ParticleSystem::EmitParticle(const ParticleSpecs& particleSpecs) {
  Particle& particle = particles_[next_particle_];
  particle.active    = true;

  particle.translation = particleSpecs.origin;
  particle.rotation    = utils::Random::Instance().Normalized() * M_PI;

  particle.velocity = particleSpecs.velocity;
  particle.velocity.x += (utils::Random::Instance().Normalized() - 0.5f) * particleSpecs.velocity_variation.x;
  particle.velocity.y += (utils::Random::Instance().Normalized() - 0.5f) * particleSpecs.velocity_variation.y;

  particle.color_begin = particleSpecs.color_begin;
  particle.color_end   = particleSpecs.color_end;

  particle.lifetime       = particleSpecs.lifetime;
  particle.time_remaining = particleSpecs.lifetime;
  particle.size_begin     = particleSpecs.size_begin;
  particle.size_end       = particleSpecs.size_end;
  particle.size_begin += particleSpecs.size_variation * (utils::Random::Instance().Normalized() - 0.5f);

  next_particle_ = (next_particle_ + 1) % particles_.size();
}

}  // namespace ra::render