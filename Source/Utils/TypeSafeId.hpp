/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file TypeSafeId.hpp
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <compare>
#include <concepts>
#include <cstdint>

namespace ra::utils {

template <typename Tag, std::unsigned_integral IntegerType = uint64_t>
class TypeSafeId {
 public:
  constexpr TypeSafeId() = default;
  constexpr explicit TypeSafeId(IntegerType value);

  constexpr IntegerType Value() const;

  auto operator<=>(const TypeSafeId<Tag, IntegerType>&) const = default;

 private:
  IntegerType value_{0};
};

template <typename Tag, std::unsigned_integral IntegerType>
constexpr TypeSafeId<Tag, IntegerType>::TypeSafeId(IntegerType value) : value_(value) {}

template <typename Tag, std::unsigned_integral IntegerType>
constexpr IntegerType TypeSafeId<Tag, IntegerType>::Value() const {
  return value_;
}

}  // namespace ra::utils

namespace std {

template <typename Tag, std::unsigned_integral IntegerType>
struct hash<ra::utils::TypeSafeId<Tag, IntegerType>> {
  size_t operator()(const auto& id) const { return hash<IntegerType>()(id.Value()); }
};

}  // namespace std
