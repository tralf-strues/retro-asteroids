/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Component.hpp
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Utils/Assert.hpp>
#include <Utils/TypeSafeBitmask.hpp>
#include <Utils/TypeSafeId.hpp>

namespace ra::ecs::detail {

class ComponentTag;

using ComponentId   = ra::utils::TypeSafeId<ComponentTag>;
using ComponentMask = ra::utils::TypeSafeBitmask<ComponentTag>;

class SequentialGenerator {
 public:
  static uint64_t Next();

 private:
  static uint64_t current_;
};

template <typename Component>
class ComponentTraits {
 public:
  static constexpr ComponentId Id();

 private:
  static uint64_t bit_index_;
};

template <typename Component>
constexpr ComponentId ComponentTraits<Component>::Id() {
  RA_ASSERT(bit_index_ < 64U, "Too many components!");
  return ComponentId(1U << bit_index_);
}

template <typename Component>
uint64_t ComponentTraits<Component>::bit_index_{SequentialGenerator::Next()};

}  // namespace ra::ecs::detail