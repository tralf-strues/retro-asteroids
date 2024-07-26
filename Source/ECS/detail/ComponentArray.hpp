/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file ComponentArray.hpp
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <ECS/detail/Component.hpp>
#include <Utils/Assert.hpp>

#include <memory>
#include <span>

namespace ra::ecs::detail {

class ComponentArray {
 public:
  ComponentArray() = default;
  explicit ComponentArray(detail::ComponentId component_id, size_t component_size);

  template <typename Component, typename... ArgTypes>
  size_t Emplace(ArgTypes&&... args);

  size_t Insert(std::span<const uint8_t> component_data);

  void Remove(size_t idx);

  [[nodiscard]] size_t Size() const;
  [[nodiscard]] detail::ComponentId ComponentId() const;
  [[nodiscard]] size_t ComponentSize() const;

  template <typename Component>
  [[nodiscard]] Component& At(size_t idx);

  [[nodiscard]] std::span<uint8_t> At(size_t idx);

 private:
  void Reallocate();

  detail::ComponentId        component_id_{0U};
  size_t                     component_size_{0U};
  size_t                     size_{0U};
  size_t                     capacity_{0U};
  std::unique_ptr<uint8_t[]> data_{nullptr};
};

template <typename Component, typename... ArgTypes>
size_t ComponentArray::Emplace(ArgTypes&&... args) {
  if (size_ + 1 >= capacity_) {
    Reallocate();
  }

  auto idx = size_++;
  new (&At<Component>(idx)) Component(std::forward<ArgTypes>(args)...);
  return idx;
}

template <typename Component>
Component& ComponentArray::At(size_t idx) {
  return *reinterpret_cast<Component*>(At(idx).data());
}

}  // namespace ra::ecs::detail