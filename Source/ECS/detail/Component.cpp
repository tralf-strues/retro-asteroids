/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Component.cpp
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 */

#include <ECS/detail/Component.hpp>

namespace ra::ecs::detail {

uint64_t SequentialGenerator::current_{0U};

uint64_t SequentialGenerator::Next() {
  return current_++;
}

}  // namespace ra::ecs::detail