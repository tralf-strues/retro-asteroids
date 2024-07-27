/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file DeferQueue.hpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <ECS/World.hpp>

#include <functional>
#include <vector>

namespace ra {

class DeferQueue {
 public:
  using DeferTask = std::function<void(ecs::World&)>;

  void Push(DeferTask task);

  void Execute(ecs::World& world);

  void Clear();

 private:
  std::vector<DeferTask> tasks_;
};

}  // namespace ra