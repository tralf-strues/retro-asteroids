/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file UpdateSystems.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Game/Components.hpp>
#include <Game/DeferQueue.hpp>
#include <Game/Prefabs.hpp>
#include <Input/Keyboard.hpp>
#include <Input/Mouse.hpp>
#include <Render/ParticleSystem.hpp>
#include <Render/Renderer.hpp>

#include <numbers>
#include <span>

namespace ra::systems {

// struct ContextProcessInput {
//   render::Renderer& renderer;
// };

void ProcessInput(render::Renderer& renderer,
                  const InputController& input_controller,
                  Transform& transform,
                  Velocity& velocity,
                  Shooting& shooting) {
  const auto rotation_matrix = math::RotationMatrix(transform.rotation);
  auto forward = math::Vec2f(rotation_matrix * math::Vec3f(0.0f, 1.0f, 1.0f));
  auto right   = math::NormalClockwise(forward);

  /* Movement */
  // auto forward = math::Vec2f(0.0f, 1.0f);
  // auto right   = math::NormalClockwise(forward);

  math::Vec2f delta_velocity{0.0f};
  if (input::CheckKey(input::Key::Left)) {
    delta_velocity -= input_controller.movement_speed.x * right;
  } else if (input::CheckKey(input::Key::Right)) {
    delta_velocity += input_controller.movement_speed.x * right;
  }

  if (input::CheckKey(input::Key::Down)) {
    delta_velocity -= input_controller.movement_speed.x * forward;
  } else if (input::CheckKey(input::Key::Up)) {
    delta_velocity += input_controller.movement_speed.x * forward;
  }

  velocity.velocity = delta_velocity;

  /* Orientation */
  auto new_mouse_position = input::GetCursorPosition();
  if (new_mouse_position) {
    auto new_forward = math::Normalize(renderer.ScreenSpaceToWorld(new_mouse_position.value()) - transform.pos);

    if (new_forward.y == 0) {
      transform.rotation = (new_forward.x >= 0) ? -std::numbers::pi_v<float> / 2 : std::numbers::pi_v<float> / 2;
    } else if (new_forward.y > 0) {
      transform.rotation = -atanf(new_forward.x / new_forward.y);
    } else {
      transform.rotation = std::numbers::pi_v<float> - atanf(new_forward.x / new_forward.y);
    }
  }

  /* Shooting */
  // shooting.shooting = input::CheckKey(input::Key::Space);
  shooting.shooting = input::CheckMouseButton(input::MouseButton::Left);
}

void Follow(ecs::World& world, const FollowTarget& target, const Transform& transform, Velocity& velocity) {
  if (!target.target) {
    return;
  }

  const auto& target_transform = world.Get<Transform>(target.target.value());
  auto        forward          = math::Normalize(target_transform.pos - transform.pos);
  velocity.velocity            = forward * 2.0f;
}

void Move(float& dt, std::span<const Velocity> velocities, std::span<Transform> transforms) {
  const auto size = velocities.size();

  for (auto i = 0U; i < size; ++i) {
    transforms[i].pos += velocities[i].velocity * dt;
  }
}

void CalculateTransforms(std::span<const Transform> transforms, std::span<TransformMatrix> matrices) {
  const auto size = transforms.size();

  for (auto i = 0U; i < size; ++i) {
    auto translation_matrix = math::TranslationMatrix(transforms[i].pos);
    auto rotation_matrix    = math::RotationMatrix(transforms[i].rotation);
    auto scale_matrix       = math::ScaleMatrix(math::Vec2f(transforms[i].scale));

    matrices[i].matrix = rotation_matrix * scale_matrix;
    matrices[i].matrix = translation_matrix * matrices[i].matrix;
  }
}

struct ContextShooting {
  DeferQueue& defer_queue;
  float       dt;
};

void Shoot(ContextShooting& context, Shooting& shooting, const Transform& transform,
           const TransformMatrix& transform_matrix) {
  if (!shooting.shooting || shooting.recharge_current > 0.0f) {
    shooting.recharge_current -= context.dt;
    return;
  }

  const auto rotation_matrix = math::RotationMatrix(transform.rotation);
  auto forward = math::Vec2f(rotation_matrix * math::Vec3f(0.0f, 1.0f, 1.0f));

  auto l_pos               = math::Vec2f(transform_matrix.matrix * math::Vec3f(shooting.ms_positions[0U], 1.0f));
  auto r_pos               = math::Vec2f(transform_matrix.matrix * math::Vec3f(shooting.ms_positions[1U], 1.0f));
  auto projectile_velocity = shooting.projectile_speed * forward;

  context.defer_queue.Push([=](ecs::World& world) {
    auto l_transform = transform;
    l_transform.pos  = l_pos;
    SpawnProjectile(world, l_transform, projectile_velocity);

    auto r_transform = transform;
    r_transform.pos  = r_pos;
    SpawnProjectile(world, r_transform, projectile_velocity);
  });

  shooting.recharge_current = shooting.recharge;
}

void EmitPlayerParticles(float &, const InputController&, const Transform& transform, const TransformMatrix& transform_matrix,
                         const Velocity& velocity, render::ParticleSystem::ParticleSpecs& particle_specs,
                         render::ParticleSystem& particle_system) {
  const auto rotation_matrix = math::RotationMatrix(transform.rotation);
  auto forward = math::Vec2f(rotation_matrix * math::Vec3f(0.0f, 1.0f, 1.0f));
  auto right   = math::NormalClockwise(forward);

  particle_specs.velocity = -forward * (Length(velocity.velocity) + 5.0f);
  particle_specs.velocity_variation = 3.0f * right + 10.0f * forward;

  math::Vec2f positions[] {
    math::Vec2f(transform_matrix.matrix * math::Vec3f(-0.535f, -0.59f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f( 0.535f, -0.59f, 1.0f))
  };

  for (const auto& pos : positions) {
    particle_specs.origin = pos;

    for (uint32_t i = 0; i < 16U; ++i) {
      particle_system.EmitParticle(particle_specs);
    }
  }
}

void EmitUFOParticles(float&, const FollowTarget&, const TransformMatrix& transform_matrix,
                      render::ParticleSystem::ParticleSpecs& particle_specs, render::ParticleSystem& particle_system) {
  particle_specs.velocity = math::Vec2f(0.0f, -15.0f);
  particle_specs.velocity_variation = math::Vec2f(15.0f, 5.0f);

  math::Vec2f positions[] {
    math::Vec2f(transform_matrix.matrix * math::Vec3f(-0.4f, -0.5f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f( 0.4f, -0.5f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f(-0.8f, -0.5f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f( 0.8f, -0.5f, 1.0f)),
  };

  for (const auto& pos : positions) {
    particle_specs.origin = pos;

    for (uint32_t i = 0; i < 7U; ++i) {
      particle_system.EmitParticle(particle_specs);
    }
  }
}

void UpdateParticles(float& dt, std::span<render::ParticleSystem> particle_systems) {
  const auto size = particle_systems.size();
  for (auto i = 0U; i < size; ++i) {
    particle_systems[i].Update(dt);
  }
}

// void CollisionDetection(ecs::World& world, SphereCollider& first_collider) {
  
// }

void DestroyOnFarAway(DeferQueue& defer_queue, const std::unordered_map<uint64_t, ecs::EntityId>& idx_to_entity,
                      std::span<const Transform> transforms) {
  constexpr float kMaxDistance = 2000.0f;

  const auto size = transforms.size();
  for (auto i = 0U; i < size; ++i) {
    if (math::LengthSquared(transforms[i].pos) < kMaxDistance * kMaxDistance) {
      continue;
    }

    defer_queue.Push([entity = idx_to_entity.at(i)](ecs::World& world) {
      world.DestroyEntity(entity);
    });
  }
}

}  // namespace ra::systems