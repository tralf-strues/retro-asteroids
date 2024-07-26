/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Entity.hpp
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Utils/TypeSafeId.hpp>

namespace ra::ecs {

namespace detail {
class EntityTag;
}  // namespace detail

using EntityId = utils::TypeSafeId<detail::EntityTag>;

}  // namespace ra::ecs
