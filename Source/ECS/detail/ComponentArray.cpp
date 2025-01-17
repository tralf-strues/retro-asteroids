/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file ComponentArray.cpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#include <ECS/detail/ComponentArray.hpp>

#include <cstring>
#include <utility>

namespace ra::ecs::detail {

ComponentArray::ComponentArray(DestructorFunc destructor, detail::ComponentId component_id, size_t component_size)
    : destructor_(destructor), component_id_(component_id), component_size_(component_size) {}

ComponentArray::~ComponentArray() {
  for (size_t i = 0U; i < size_; ++i) {
    destructor_(At(i).data());
  }
}

ComponentArray::ComponentArray(ComponentArray&& other) noexcept
    : destructor_(std::exchange(other.destructor_, nullptr)),
      component_id_(other.component_id_),
      component_size_(other.component_size_),
      size_(std::exchange(other.size_, 0U)),
      capacity_(std::exchange(other.capacity_, 0U)),
      data_(std::exchange(other.data_, nullptr)) {}

ComponentArray& ComponentArray::operator=(ComponentArray&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  std::swap(destructor_, other.destructor_);
  std::swap(component_id_, other.component_id_);
  std::swap(component_size_, other.component_size_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
  std::swap(data_, other.data_);

  return *this;
}

size_t ComponentArray::Insert(std::span<const uint8_t> component_data) {
  RA_ASSERT(component_data.size_bytes() == component_size_,
            "Trying to insert a component of not matching size (internal component size = %zu, inserting component "
            "size = %zu)",
            component_size_, component_data.size_bytes());

  if (size_ + 1 >= capacity_) {
    Reallocate();
  }

  auto idx = size_++;
  std::memcpy(At(idx).data(), component_data.data(), component_size_);
  return idx;
}

void ComponentArray::Remove(size_t idx, bool destroy) {
  RA_ASSERT(idx < size_, "ComponentArray access out of bounds (idx = %zu, size = %zu)", idx, size_);
  if (idx == size_ - 1U) {
    --size_;
    return;
  }

  auto to_remove = At(idx);
  if (destroy) {
    destructor_(to_remove.data());
  }

  auto last_elem = At(Size() - 1U);
  std::memcpy(to_remove.data(), last_elem.data(), component_size_);

  --size_;
}

ComponentArray::DestructorFunc ComponentArray::Destructor() const {
  return destructor_;
}

size_t ComponentArray::Size() const { return size_; }

detail::ComponentId ComponentArray::ComponentId() const {
  return component_id_;
}

size_t ComponentArray::ComponentSize() const { return component_size_; }

std::span<uint8_t> ComponentArray::At(size_t idx) {
  RA_ASSERT(idx < size_, "ComponentArray access out of bounds (idx = %zu, size = %zu)", idx, size_);

  return std::span(data_.get() + idx * component_size_, component_size_);
}

std::span<uint8_t> ComponentArray::Data() {
  return std::span(data_.get(), size_ * component_size_);
}

void ComponentArray::Reallocate() {
  auto new_capacity = (capacity_ == 0U) ? 16U : capacity_ * 2U;

  auto new_data = std::make_unique<uint8_t[]>(new_capacity * component_size_);
  if (data_ && size_ > 0U) {
    std::memcpy(new_data.get(), data_.get(), size_ * component_size_);
  }

  capacity_ = new_capacity;
  data_ = std::move(new_data);
}

}  // namespace ra::ecs::detail