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
#include <Utils/Random.hpp>

#include <numbers>
#include <span>

namespace ra::systems {

void ProcessInput(render::Renderer& renderer,
                  const InputController& input_controller,
                  Transform& transform,
                  Velocity& velocity,
                  Shooting& shooting) {
  const auto rotation_matrix = math::RotationMatrix(transform.rotation);
  auto forward = math::Vec2f(rotation_matrix * math::Vec3f(0.0f, 1.0f, 1.0f));
  auto right   = math::NormalClockwise(forward);

  /* Movement */
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
  shooting.shooting = input::CheckMouseButton(input::MouseButton::Left);
}

void Follow(ecs::World& world, const FollowTarget& target, const Transform& transform, Velocity& velocity) {
  if (!target.target) {
    return;
  }

  const auto& target_transform = world.Get<Transform>(target.target.value());
  auto        forward          = math::Normalize(target_transform.pos - transform.pos);
  velocity.velocity            = forward * target.speed;
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

void UpdateColliders(std::span<const Transform> transforms, std::span<const TransformMatrix> transform_matrices,
                     std::span<SphereCollider> sphere_collider) {
  const auto size = transforms.size();

  for (auto i = 0U; i < size; ++i) {
    auto& collider = sphere_collider[i];

    collider.ws_pos    = math::Vec2f(transform_matrices[i].matrix * math::Vec3f(collider.ms_pos, 1.0f));
    collider.ws_radius = collider.ms_radius * transforms[i].scale;
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

  particle_specs.velocity = -forward * (Length(velocity.velocity) + 3.0f);
  particle_specs.velocity_variation = 3.0f * right + 5.0f * forward;

  math::Vec2f positions[] {
    math::Vec2f(transform_matrix.matrix * math::Vec3f(-0.535f, -0.59f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f( 0.535f, -0.59f, 1.0f))
  };

  for (const auto& pos : positions) {
    particle_specs.origin = pos;

    for (uint32_t i = 0; i < 22U; ++i) {
      particle_system.EmitParticle(particle_specs);
    }
  }
}

void EmitUFOParticles(float&, const FollowTarget&, const TransformMatrix& transform_matrix,
                      render::ParticleSystem::ParticleSpecs& particle_specs, render::ParticleSystem& particle_system) {
  particle_specs.velocity           = math::Vec2f(0.0f, -5.0f);
  particle_specs.velocity_variation = math::Vec2f(5.0f, 2.0f);

  math::Vec2f positions[] {
    math::Vec2f(transform_matrix.matrix * math::Vec3f(-0.4f, -0.5f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f( 0.4f, -0.5f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f(-0.8f, -0.5f, 1.0f)),
    math::Vec2f(transform_matrix.matrix * math::Vec3f( 0.8f, -0.5f, 1.0f)),
  };

  for (const auto& pos : positions) {
    particle_specs.origin = pos;

    for (uint32_t i = 0; i < 12U; ++i) {
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

struct ContextCollisionDetection {
  ecs::World&                            world;
  DeferQueue&                            defer_queue;
  uint32_t&                              score;
  int32_t&                               enemies_left;
  bool&                                  game_over;
  render::ParticleSystem&                explosions;
  render::ParticleSystem::ParticleSpecs& explosion_specs;
};

void CollisionDetection(ContextCollisionDetection& context, ecs::EntityId first, ecs::EntityId second) {
  auto&       world         = context.world;
  const auto& first_sphere  = world.Get<SphereCollider>(first);
  const auto& second_sphere = world.Get<SphereCollider>(second);

  if (!SpheresCollide(first_sphere, second_sphere)) {
    return;
  }

  auto first_team  = world.Get<TeamTag>(first);
  auto second_team = world.Get<TeamTag>(second);

  if (first_team == second_team) {
    return;
  }

  if (second_team < first_team) {
    std::swap(first, second);
    std::swap(first_team, second_team);
  }

  if (world.Has<InputController>(first)) {
    context.defer_queue.Push([first, &game_over = context.game_over](ecs::World& world) {
      world.DestroyEntity(first);
      game_over = true;
    });
  } else if (auto* damage = world.TryGet<Damage>(first); damage) {
    auto& health = world.Get<Health>(second);
    health.value -= damage->value;

    auto        explosion_position = world.Get<Transform>(second).pos;
    auto&       random_gen         = utils::Random::Instance();
    math::Vec2f area(2.0f, 2.0f);

    context.explosion_specs.size_begin         = 0.004f;
    context.explosion_specs.size_end           = 0.001f;
    context.explosion_specs.size_variation     = 0.001f;
    context.explosion_specs.lifetime           = 0.1f;
    context.explosion_specs.velocity_variation = math::Vec2f(100.0f);

    for (uint32_t i = 0U; i < 8U; ++i) {
      math::Vec2f random_dir{random_gen.InRange(-0.5f, 0.5f), random_gen.InRange(-0.5f, 0.5f)};
      math::Vec2f random_offset{random_gen.InRange(-0.5f, 0.5f), random_gen.InRange(-0.5f, 0.5f)};

      context.explosion_specs.origin   = explosion_position + area * random_offset;
      context.explosion_specs.velocity = 3.0f * random_dir;

      context.explosions.EmitParticle(context.explosion_specs);
    }

    context.defer_queue.Push([first](ecs::World& world) {
      world.DestroyEntity(first);
    });

    if (health.value <= 0) {
      context.score += 10;
      --context.enemies_left;

      area = math::Vec2f(3.0f, 3.0f);

      context.explosion_specs.size_begin         = 0.01f;
      context.explosion_specs.size_end           = 0.005f;
      context.explosion_specs.size_variation     = 0.005f;
      context.explosion_specs.lifetime           = 0.2f;
      context.explosion_specs.velocity_variation = math::Vec2f(75.0f);

      for (uint32_t i = 0U; i < 32U; ++i) {
        math::Vec2f random_dir{random_gen.InRange(-0.5f, 0.5f), random_gen.InRange(-0.5f, 0.5f)};
        math::Vec2f random_offset{random_gen.InRange(-0.5f, 0.5f), random_gen.InRange(-0.5f, 0.5f)};

        context.explosion_specs.origin   = explosion_position + area * random_offset;
        context.explosion_specs.velocity = 3.0f * random_dir;

        context.explosions.EmitParticle(context.explosion_specs);
      }

      context.defer_queue.Push([second](ecs::World& world) {
        world.DestroyEntity(second);
      });
    }
  }
}

struct ContextSpawnNewEnemies {
  DeferQueue&   defer_queue;
  ecs::EntityId target;
  int32_t&      enemy_level;
  int32_t&      enemies_left;
};

void SpawnNewEnemies(ContextSpawnNewEnemies& context, const InputController&, const SphereCollider& collider) {
  if (context.enemies_left > 0) {
    return;
  }

  auto& random_gen = utils::Random::Instance();

  ++context.enemy_level;
  context.enemies_left = 0;

  SphereCollider safe_spawn_sphere = collider;
  safe_spawn_sphere.ws_radius += 15.0f;

  int32_t to_spawn = random_gen.InRange(3, 10);
  for (int32_t i = 0; i < to_spawn; ++i) {
    SphereCollider ufo_sphere;
    math::Vec2f    pos;
    do {
      pos = math::Vec2f(random_gen.InRange(-40, 40), random_gen.InRange(-40, 40));

      ufo_sphere.ws_pos    = math::Vec2f(0.0f, 0.2f) * 18.0f + pos;
      ufo_sphere.ws_radius = std::sqrt(1.73f) * 18.0f;
    } while (SpheresCollide(ufo_sphere, safe_spawn_sphere));

    auto speed = std::min(3.0f + (context.enemy_level - 1U) * 0.75f, 20.0f);

    context.defer_queue.Push([pos, speed, target = context.target](ecs::World& world) {
      SpawnUFO(
        world, pos, speed, target);
    });
  }

  context.enemies_left = to_spawn;
}

void DestroyOnFarAway(DeferQueue& defer_queue, const std::unordered_map<uint64_t, ecs::EntityId>& idx_to_entity,
                      std::span<const Transform> transforms) {
  constexpr float kMaxDistance = 200.0f;

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