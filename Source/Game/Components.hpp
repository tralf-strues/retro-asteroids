/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Components.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Asset/FontAtlas.hpp>
#include <ECS/Entity.hpp>
#include <Math/Mat3.hpp>
#include <Render/Polygon.hpp>

#include <memory>

namespace ra {

/* Update components */
struct InputController {
  math::Vec2f movement_speed{10.0f, 20.0f};
};

struct Velocity {
  math::Vec2f velocity{0.0f};
};

struct Shooting {
  math::Vec2f ms_positions[2U];
  float recharge{0.0f};
  float projectile_speed{0.0f};

  bool  shooting{false};
  float recharge_current{0.0f};
};

struct Transform {
  math::Vec2f pos{0.0f};
  float       rotation{0.0f};
  float       scale{1.0f};
};

struct TransformMatrix {
  math::Mat3f matrix;
};

struct FollowTarget {
  std::optional<ecs::EntityId> target{std::nullopt};
  float                        speed;
};

struct SphereCollider {
  math::Vec2f ms_pos;
  float       ms_radius;

  math::Vec2f ws_pos;
  float       ws_radius;
};

constexpr bool SpheresCollide(const SphereCollider& first, const SphereCollider& second) {
  return math::LengthSquared(second.ws_pos - first.ws_pos) <=
         (second.ws_radius + first.ws_radius) * (second.ws_radius + first.ws_radius);
}

enum class TeamTag : uint32_t {
  Player,
  Enemy
};

struct Health {
  int32_t value;
};

struct Damage {
  int32_t value;
};

/* Render components */
struct PolygonRenderer {
  std::shared_ptr<render::Polygon> polygon;
};

}  // namespace ra