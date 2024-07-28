/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file prefabs.hpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <ECS/World.hpp>
#include <Math/Vec2.hpp>

namespace ra {

struct Transform;

ecs::EntityId SpawnPlayer(ecs::World& world);
ecs::EntityId SpawnUFO(ecs::World& world, math::Vec2f pos, float speed, ecs::EntityId target);

ecs::EntityId SpawnProjectile(ecs::World& world, const Transform& transform, math::Vec2f velocity);

}  // namespace ra