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

/* Id */
class ComponentTag;
using ComponentId = ra::utils::TypeSafeId<ComponentTag>;

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

/* Mask */
using ComponentMask = ra::utils::TypeSafeBitmask<ComponentTag>;

template <typename Component, typename... Components>
constexpr ComponentMask ComponentMaskOf() noexcept {
  if constexpr (sizeof...(Components) == 0) {
    return ComponentMask(ComponentTraits<std::remove_cv_t<Component>>::Id().Value());
  } else {
    return ComponentMask(ComponentTraits<std::remove_cv_t<Component>>::Id().Value()) |
           ComponentMaskOf<std::remove_cv_t<Components>...>();
  }
}

}  // namespace ra::ecs::detail