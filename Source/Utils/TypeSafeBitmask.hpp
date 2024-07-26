/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file TypeSafeBitmask.hpp
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 */

#include <concepts>
#include <cstdint>

namespace ra::utils {

template <typename Tag, std::unsigned_integral IntegerType = uint64_t>
class TypeSafeBitmask {
 public:
  constexpr TypeSafeBitmask() = default;
  constexpr explicit TypeSafeBitmask(IntegerType value);

  constexpr IntegerType Value() const;

  constexpr bool Has(TypeSafeBitmask<Tag, IntegerType> other) const;

  constexpr TypeSafeBitmask<Tag, IntegerType> Without(TypeSafeBitmask<Tag, IntegerType> other) const;

  auto operator<=>(const TypeSafeBitmask<Tag, IntegerType>&) const = default;

 private:
  IntegerType value_{0};
};

template <typename Tag, std::unsigned_integral IntegerType>
constexpr TypeSafeBitmask<Tag, IntegerType>::TypeSafeBitmask(IntegerType value) : value_(value) {}

template <typename Tag, std::unsigned_integral IntegerType>
constexpr IntegerType TypeSafeBitmask<Tag, IntegerType>::Value() const {
  return value_;
}

template <typename Tag, std::unsigned_integral IntegerType>
constexpr bool TypeSafeBitmask<Tag, IntegerType>::Has(TypeSafeBitmask<Tag, IntegerType> other) const {
  return (value_ & other.value_) == other.value_;
}

template <typename Tag, std::unsigned_integral IntegerType>
constexpr TypeSafeBitmask<Tag, IntegerType> TypeSafeBitmask<Tag, IntegerType>::Without(
    TypeSafeBitmask<Tag, IntegerType> other) const {
  return TypeSafeBitmask<Tag, IntegerType>(value_ & (~other.value_));
}

template <typename Tag, std::unsigned_integral IntegerType>
constexpr TypeSafeBitmask<Tag, IntegerType> operator|(const TypeSafeBitmask<Tag, IntegerType>& lhs,
                                                      const TypeSafeBitmask<Tag, IntegerType>& rhs) {
  return TypeSafeBitmask<Tag, IntegerType>(lhs.Value() | rhs.Value());
}

template <typename Tag, std::unsigned_integral IntegerType>
constexpr TypeSafeBitmask<Tag, IntegerType> operator^(const TypeSafeBitmask<Tag, IntegerType>& lhs,
                                                      const TypeSafeBitmask<Tag, IntegerType>& rhs) {
  return TypeSafeBitmask<Tag, IntegerType>(lhs.Value() ^ rhs.Value());
}

}  // namespace ra::utils

namespace std {

template <typename Tag, std::unsigned_integral IntegerType>
struct hash<ra::utils::TypeSafeBitmask<Tag, IntegerType>> {
  size_t operator()(const auto& id) const { return hash<IntegerType>()(id.Value()); }
};

}  // namespace std
