/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file World.cpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#include <ECS/World.hpp>

namespace ra::ecs {

static const auto kNullComponentMask = detail::ComponentMask(0U);

World::World() {
  auto null_archetype = std::make_unique<Archetype>();
  null_archetype->component_mask = kNullComponentMask;
  archetype_registry_[kNullComponentMask] = std::move(null_archetype);
}

EntityId World::NewEntity() {
  const auto entity_id = EntityId(next_entity_id_++);

  entity_registry_[entity_id] = {
    .archetype = archetype_registry_.at(kNullComponentMask).get(),
    .idx       = 0U
  };

  return entity_id;
}

void World::DestroyEntity(EntityId entity) {
  RemoveEntityRecord(entity_registry_[entity]);
  entity_registry_.erase(entity);
}

// void World::DumpStorageLayout(std::ostream os) {
//   for (const auto& [component_mask, archetype] : archetype_registry_) {
//     os << "Archetype 0b" << 
//   }
// }

World::Archetype* World::FindArchetype(detail::ComponentMask component_mask) {
  auto it = archetype_registry_.find(component_mask);
  return (it != archetype_registry_.end()) ? it->second.get() : nullptr;
}

World::Archetype* World::CreateArchetype(Archetype* old_archetype, detail::ComponentMask new_mask,
                                         size_t component_size) {
  auto old_mask = old_archetype->component_mask;
  bool is_add   = new_mask.Has(old_mask);
  auto comp_id  = detail::ComponentId((old_mask ^ new_mask).Value());

  auto new_archetype = std::make_unique<Archetype>();
  new_archetype->component_mask = new_mask;

  auto old_components_count = old_archetype->component_arrays.size();
  new_archetype->component_arrays.reserve(old_components_count + 1U);

  for (size_t old_array_idx = 0U; old_array_idx < old_components_count; ++old_array_idx) {
    const auto old_id = old_archetype->component_arrays[old_array_idx].ComponentId();
    if (!is_add && old_id == comp_id) {
      continue;
    }

    const auto new_array_idx = new_archetype->component_arrays.size();
    component_registry_[old_id][new_archetype.get()] = new_array_idx;
    new_archetype->component_arrays.emplace_back(old_id,
                                                 old_archetype->component_arrays[old_array_idx].ComponentSize());
  }

  if (is_add) {
    component_registry_[comp_id][new_archetype.get()] = old_components_count;
    new_archetype->component_arrays.emplace_back(comp_id, component_size);
  }

  auto emplaced = archetype_registry_.emplace(new_mask, std::move(new_archetype));
  return emplaced.first->second.get();
}

void World::RemoveEntityRecord(EntityRecord record) {
  auto& archetype = *record.archetype;

  if (archetype.component_arrays.empty()) {
    return;
  }

  for (auto& component_array : archetype.component_arrays) {
    component_array.Remove(record.idx);
  }

  const auto last_idx    = archetype.component_arrays[0U].Size() - 1U;
  const auto last_entity = archetype.idx_to_entity[last_idx];

  if (record.idx != last_idx) {
    archetype.idx_to_entity[record.idx] = last_entity;
    entity_registry_[last_entity].idx   = record.idx;
  }
}

}  // namespace ra::ecs