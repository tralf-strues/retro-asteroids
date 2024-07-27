/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file DeferQueue.cpp
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 */

#include <Game/DeferQueue.hpp>

namespace ra {

void DeferQueue::Push(DeferTask task) {
  tasks_.push_back(std::move(task));
}

void DeferQueue::Execute(ecs::World& world) {
  for (auto& task : tasks_) {
    task(world);
  }
}

void DeferQueue::Clear() {
  tasks_.clear();
}

}  // namespace ra