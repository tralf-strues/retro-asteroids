/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file prefabs.cpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#include <Game/Prefabs.hpp>

#include <Asset/PolygonLoader.hpp>
#include <Game/Components.hpp>
#include <Render/ParticleSystem.hpp>

namespace ra {

static const auto kPlayerPolygon    = asset::LoadPolygon("Assets/Polygons/player_spaceship.txt");
static const auto kUFOPolygon       = asset::LoadPolygon("Assets/Polygons/ufo.txt");
static const auto kPlayerProjectile = asset::LoadPolygon("Assets/Polygons/player_spaceship_projectile.txt");

ecs::EntityId SpawnPlayer(ecs::World& world) {
  auto player = world.NewEntity();

  world.Add<InputController>(player);
  world.Add<Transform>(player);
  world.Add<TransformMatrix>(player);
  world.Add<Velocity>(player);
  world.Add<TeamTag>(player) = TeamTag::Player;
  world.Add<PolygonRenderer>(player).polygon = kPlayerPolygon;

  auto& sphere_collider     = world.Add<SphereCollider>(player);
  sphere_collider.ms_pos    = math::Vec2f(0.0f, 0.7f);
  sphere_collider.ms_radius = std::sqrt(2.2525f);

  auto& particle_system = world.Add<render::ParticleSystem>(player);
  particle_system       = render::ParticleSystem(2048U);

  auto& particle_specs          = world.Add<render::ParticleSystem::ParticleSpecs>(player);
  particle_specs.color_begin    = math::Vec4f(0.05f, 0.2f, 0.8f, 1.0f);
  particle_specs.color_end      = math::Vec4f(0.2f, 0.6f, 0.8f, 1.0f);
  particle_specs.size_begin     = 0.09f;
  particle_specs.size_end       = 0.04f;
  particle_specs.size_variation = 0.03f;
  particle_specs.lifetime       = 0.1f;

  auto& shooting = world.Add<Shooting>(player);
  shooting.ms_positions[0U] = math::Vec2f(-1.15f, 1.005f);
  shooting.ms_positions[1U] = math::Vec2f( 1.15f, 1.005f);
  shooting.recharge         = 0.15f;
  shooting.projectile_speed = 50.0f;

  return player;
}

ecs::EntityId SpawnUFO(ecs::World& world, math::Vec2f pos, float speed, ecs::EntityId target) {
  auto ufo = world.NewEntity();

  world.Add<FollowTarget>(ufo)  = {.target = target, .speed = speed};
  world.Add<Transform>(ufo).pos = pos;
  world.Add<TransformMatrix>(ufo);
  world.Add<Velocity>(ufo);
  world.Add<TeamTag>(ufo)      = TeamTag::Enemy;
  world.Add<Health>(ufo).value = 100;

  world.Add<PolygonRenderer>(ufo).polygon = kUFOPolygon;

  auto& sphere_collider     = world.Add<SphereCollider>(ufo);
  sphere_collider.ms_pos    = math::Vec2f(0.0f, 0.2f);
  sphere_collider.ms_radius = std::sqrt(1.73f);

  auto& particle_system = world.Add<render::ParticleSystem>(ufo);
  particle_system       = render::ParticleSystem(2048U);

  auto& particle_specs          = world.Add<render::ParticleSystem::ParticleSpecs>(ufo);
  particle_specs.color_begin    = math::Vec4f(0.4f, 0.8f, 0.2f, 1.0f);
  particle_specs.color_end      = math::Vec4f(0.4f, 0.6f, 0.4f, 0.8f);
  particle_specs.size_begin     = 0.01f;
  particle_specs.size_end       = 0.003f;
  particle_specs.size_variation = 0.002f;
  particle_specs.lifetime       = 0.1f;

  return ufo;
}

ecs::EntityId SpawnProjectile(ecs::World& world, const Transform& transform, math::Vec2f velocity) {
  auto projectile = world.NewEntity();

  world.Add<Transform>(projectile) = transform;
  world.Add<TransformMatrix>(projectile);
  world.Add<PolygonRenderer>(projectile).polygon = kPlayerProjectile;
  world.Add<Velocity>(projectile).velocity       = velocity;
  world.Add<TeamTag>(projectile)                 = TeamTag::Player;
  world.Add<Damage>(projectile).value            = 25;

  auto& sphere_collider     = world.Add<SphereCollider>(projectile);
  sphere_collider.ms_pos    = math::Vec2f(0.0f, 0.0f);
  sphere_collider.ms_radius = std::sqrt(0.3f);

  return projectile;
}

}  // namespace ra