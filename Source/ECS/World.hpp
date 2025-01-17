/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file World.hpp
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <ECS/Entity.hpp>
#include <ECS/System.hpp>
#include <ECS/detail/Component.hpp>
#include <ECS/detail/ComponentArray.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

namespace ra::ecs {

/**
 * The following ECS is quite simple, yet capable to handle all the needs of this game.
 *
 * 1. Identifiers
 * One major constraint of this implementation is that !there can only be 64 different components!. This is done both
 * because it's simpler and more performant. Considering the scope of the game, we actually don't need even that many
 * components, so in no way does this design decision really constrain me when implementing game play.
 *
 * From the API perspective, entities are just integer ids, and components are simple POD structs somehow associated
 * with these entities. Component ids are not exposed directly through the API.
 *
 * 2. Component storage
 * Suppose we have components
 *  - Component Transform with id = 0b0001
 *  - Component Velocity  with id = 0b0010
 *  - Component Health    with id = 0b0100
 *  - Component Strength  with id = 0b1000
 *
 * And the following archetypes
 *  - "Rock":   [Transform],                             ComponentMask = 0b0001
 *  - "Wheel":  [Transform, Velocity],                   ComponentMask = 0b0011
 *  - "Aura":   [Strength],                              ComponentMask = 0b1000
 *  - "Bird":   [Transform, Velocity, Health],           ComponentMask = 0b0111
 *  - "Player": [Transform, Velocity, Health, Strength], ComponentMask = 0b1111
 *
 * The layout therefore is the following:
 * ------------------------------------
 * Archetype "Rock"   array [T T T ...]
 * ------------------------------------
 * ------------------------------------
 * Archetype "Wheel"  array [T T T ...]
 *                    array [V V V ...]
 * ------------------------------------
 * ------------------------------------
 * Archetype "Aura"   array [S S S ...]
 * ------------------------------------
 * ------------------------------------
 * Archetype "Bird"   array [T T T ...]
 *                    array [V V V ...]
 *                    array [H H H ...]
 * ------------------------------------
 * ------------------------------------
 * Archetype "Player" array [T T T ...]
 *                    array [V V V ...]
 *                    array [H H H ...]
 *                    array [S S S ...]
 * ------------------------------------
 *
 * Entity world is simply speaking just a map from `EntityId` to `pair<Archetype, std::size_t>`, where the
 * latter is the actual index into the archetype's multi-array.
 */
class World {
 public:
  World();

  [[nodiscard]] EntityId NewEntity();
  void DestroyEntity(EntityId entity);

  template <typename Component>
  [[nodiscard]] bool Has(EntityId entity) const;

  template <typename Component>
  [[nodiscard]] Component& Get(EntityId entity);

  template <typename Component>
  [[nodiscard]] Component* TryGet(EntityId entity);

  template <typename Component, typename... ArgTypes>
  Component& Add(EntityId entity, ArgTypes&&... args);

  template <typename Component>
  void Remove(EntityId entity);

  template <typename... Components>
  void Run(FreeSystem<Components...> system);

  template <typename Context, typename... Components>
  void Run(Context& context, SystemSingle<Context, Components...> system);

  template <typename Context, typename... Components>
  void Run(Context& context, SystemVector<Context, Components...> system);

  template <typename Context, typename... Components>
  void Run(Context& context, MegaSystemVector<Context, Components...> system);

  template <typename Context, typename... Components>
  void RunInteractions(Context& context, InteractionSystem<Context, Components...> system);

 private:
  struct Archetype {
    using ComponentArrays = std::vector<detail::ComponentArray>;
    using MapIdxToEntity  = std::unordered_map<uint64_t, EntityId>;

    detail::ComponentMask component_mask;
    ComponentArrays       component_arrays;
    MapIdxToEntity        idx_to_entity;
  };

  struct EntityRecord {
    Archetype* archetype;
    uint64_t   idx;
  };

  using ArchetypeRegistry = std::unordered_map<detail::ComponentMask, std::unique_ptr<Archetype>>;

  using EntityRegistry = std::unordered_map<EntityId, EntityRecord>;

  using ComponentRecords  = std::unordered_map<Archetype*, uint64_t>;
  using ComponentRegistry = std::unordered_map<detail::ComponentId, ComponentRecords>;

  Archetype* FindArchetype(detail::ComponentMask component_mask);
  Archetype* CreateArchetype(Archetype* old_archetype, detail::ComponentMask new_mask, size_t component_size,
                             detail::ComponentArray::DestructorFunc destructor);

  void RemoveEntityRecord(EntityRecord record, bool destroy_component);

  template <typename Component>
  std::span<Component> QueryComponent(Archetype& archetype);

  template <typename Context, typename... Components>
  void RunInteractionsInternal(Context& context, InteractionSystem<Context, Components...>, EntityId first_entity,
                               ArchetypeRegistry::const_iterator         it_archetypes,
                               Archetype::MapIdxToEntity::const_iterator it_entity_mappings);

  ArchetypeRegistry archetype_registry_;
  EntityRegistry    entity_registry_;
  ComponentRegistry component_registry_;
  uint64_t          next_entity_id_{0U};
};

template <typename Component>
bool World::Has(EntityId entity) const {
  static const detail::ComponentId kComponentId = detail::ComponentTraits<Component>::Id();

  auto*       archetype         = entity_registry_.at(entity).archetype;
  const auto& component_records = component_registry_.at(kComponentId);

  return component_records.contains(archetype);
}

template <typename Component>
Component& World::Get(EntityId entity) {
  return *TryGet<Component>(entity);
}

template <typename Component>
Component* World::TryGet(EntityId entity) {
  static const detail::ComponentId kComponentId = detail::ComponentTraits<Component>::Id();

  auto  entity_record = entity_registry_[entity];
  auto* archetype     = entity_record.archetype;

  auto& component_records   = component_registry_.at(kComponentId);
  auto  component_record_it = component_records.find(archetype);
  if (component_record_it == component_records.end()) {
    return nullptr;
  }

  auto component_record = component_record_it->second;
  return &archetype->component_arrays[component_record].template At<Component>(entity_record.idx);
}

template <typename Component, typename... ArgTypes>
Component& World::Add(EntityId entity, ArgTypes&&... args) {
  static const detail::ComponentId kComponentId = detail::ComponentTraits<Component>::Id();

  auto  old_record    = entity_registry_[entity];
  auto* old_archetype = old_record.archetype;

  auto  new_component_mask = old_archetype->component_mask | detail::ComponentMask(kComponentId.Value());
  auto* new_archetype      = FindArchetype(new_component_mask);
  if (!new_archetype) {
    new_archetype = CreateArchetype(old_archetype, new_component_mask, sizeof(Component),
                                    &detail::TypeErasedDestructor<Component>);
  }

  // Move all overlapping components from old archetype's record to new one
  for (auto& old_array : old_archetype->component_arrays) {
    const auto& component_records = component_registry_.at(old_array.ComponentId());
    auto comp_idx = component_records.at(new_archetype);

    new_archetype->component_arrays[comp_idx].Insert(old_array.At(old_record.idx));
  }

  // Add new component to new archetype's record
  auto comp_idx   = component_registry_.at(kComponentId).at(new_archetype);
  auto entity_idx = new_archetype->component_arrays[comp_idx].Emplace<Component>(std::forward<ArgTypes>(args)...);
  new_archetype->idx_to_entity[entity_idx] = entity;
  entity_registry_[entity] = EntityRecord{.archetype = new_archetype, .idx = entity_idx};

  // Remove entity record from old archetype
  RemoveEntityRecord(old_record, false);

  return Get<Component>(entity);
}

template <typename Component>
void World::Remove(EntityId entity) {
  static const detail::ComponentId kComponentId = detail::ComponentTraits<Component>::Id();

  auto  old_record    = entity_registry_[entity];
  auto* old_archetype = old_record.archetype;

  auto  new_component_mask = old_archetype->component_mask.Without(detail::ComponentMask(kComponentId.Value()));
  auto* new_archetype      = FindArchetype(new_component_mask);
  if (!new_archetype) {
    new_archetype = CreateArchetype(&detail::TypeErasedDestructor<Component>, old_archetype,
                                    new_component_mask, sizeof(Component));
  }

  // Move all overlapping components from old archetype's record to new one
  uint64_t entity_idx = 0U;
  for (auto& new_array : new_archetype->component_arrays) {
    const auto& component_records = component_registry_.at(new_array.ComponentId());
    auto comp_idx = component_records.at(old_archetype);

    entity_idx = new_array.Insert(old_archetype->component_arrays[comp_idx].At(old_record.idx));
  }

  new_archetype->idx_to_entity[entity_idx] = entity;
  entity_registry_[entity] = EntityRecord{.archetype = new_archetype, .idx = entity_idx};

  // Remove entity record from old archetype
  RemoveEntityRecord(old_record, true);
}

template <typename... Components>
void World::Run(FreeSystem<Components...> system) {
  static const detail::ComponentMask kComponentMask = detail::ComponentMaskOf<std::remove_cv_t<Components>...>();

  for (const auto& [archetype_mask, archetype] : archetype_registry_) {
    if (archetype_mask.Has(kComponentMask)) {
      system(QueryComponent<Components>(*archetype.get())...);
    }
  }
}

template <typename Context, typename... Components>
void World::Run(Context& context, SystemSingle<Context, Components...> system) {
  struct ExtendedContext {
    SystemSingle<Context, Components...>& single_system;
    Context&                              single_context;
  } extended_context {
    .single_system  = system,
    .single_context = context
  };

  Run(extended_context, &detail::DefaultVectorSystem<ExtendedContext, Components...>);
}

template <typename Context, typename... Components>
void World::Run(Context& context, SystemVector<Context, Components...> system) {
  static const detail::ComponentMask kComponentMask = detail::ComponentMaskOf<std::remove_cv_t<Components>...>();

  for (const auto& [archetype_mask, archetype] : archetype_registry_) {
    if (archetype_mask.Has(kComponentMask)) {
      system(context, QueryComponent<Components>(*archetype.get())...);
    }
  }
}

template <typename Context, typename... Components>
void World::Run(Context& context, MegaSystemVector<Context, Components...> system) {
  static const detail::ComponentMask kComponentMask = detail::ComponentMaskOf<std::remove_cv_t<Components>...>();

  for (const auto& [archetype_mask, archetype] : archetype_registry_) {
    if (archetype_mask.Has(kComponentMask)) {
      system(context, archetype->idx_to_entity, QueryComponent<Components>(*archetype.get())...);
    }
  }
}

template <typename Context, typename... Components>
void World::RunInteractions(Context& context, InteractionSystem<Context, Components...> system) {
  static const detail::ComponentMask kComponentMask = detail::ComponentMaskOf<std::remove_cv_t<Components>...>();

  for (auto archetype_it = archetype_registry_.begin(); archetype_it != archetype_registry_.end(); ++archetype_it) {
    const auto& [archetype_mask, archetype] = *archetype_it;
    if (!archetype_mask.Has(kComponentMask)) {
      continue;
    }

    auto& idx_to_entity = archetype->idx_to_entity;
    for (auto it_entity = idx_to_entity.begin(); it_entity != idx_to_entity.end(); ++it_entity) {
      RunInteractionsInternal<Context, Components...>(context, system, it_entity->second, archetype_it, it_entity);
    }
  }
}

template <typename Context, typename... Components>
void World::RunInteractionsInternal(Context& context, InteractionSystem<Context, Components...> system,
                                    EntityId first_entity, ArchetypeRegistry::const_iterator it_archetypes,
                                    Archetype::MapIdxToEntity::const_iterator it_entity_mappings) {
  static const detail::ComponentMask kComponentMask = detail::ComponentMaskOf<std::remove_cv_t<Components>...>();

  for (auto archetype_it = it_archetypes; archetype_it != archetype_registry_.end(); ++archetype_it) {
    const auto& [archetype_mask, archetype] = *archetype_it;
    if (!archetype_mask.Has(kComponentMask)) {
      continue;
    }

    auto& idx_to_entity = archetype->idx_to_entity;
    auto  it_entity     = (archetype_it == it_archetypes) ? it_entity_mappings : idx_to_entity.begin();
    for (; it_entity != idx_to_entity.end(); ++it_entity) {
      if (first_entity != it_entity->second) {
        system(context, first_entity, it_entity->second);
      }
    }
  }
}

template <typename Component>
std::span<Component> World::QueryComponent(Archetype& archetype) {
  static const detail::ComponentId kComponentId = detail::ComponentTraits<std::remove_cv_t<Component>>::Id();

  auto const comp_idx = component_registry_[kComponentId][&archetype];
  return archetype.component_arrays[comp_idx].template Data<Component>();
}

}  // namespace ra::ecs