/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file System.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <ECS/Entity.hpp>

#include <span>
#include <unordered_map>

namespace ra::ecs {

template <typename... Components>
using FreeSystem = void(*)(std::span<Components>...);

template <typename Context, typename... Components>
using SystemSingle = void(*)(Context& context, Components&...);

template <typename Context, typename... Components>
using SystemVector = void(*)(Context& context, std::span<Components>...);

template <typename Context, typename... Components>
using MegaSystemVector = void (*)(Context& context, const std::unordered_map<uint64_t, EntityId>& idx_to_entity,
                                  std::span<Components>...);

template <typename Context, typename... Components>
using InteractionSystem = void (*)(Context& context, EntityId first_entity, EntityId second_entity);

namespace detail {

template <typename Context, typename FirstComponent, typename... Components>
void DefaultVectorSystem(Context& context, std::span<FirstComponent> first, std::span<Components>... other) {
  const auto size = first.size();

  for (size_t i = 0U; i < size; ++i) {
    context.single_system(context.single_context, first[i], other[i]...);
  }
}

}

}  // namespace ra::ecs