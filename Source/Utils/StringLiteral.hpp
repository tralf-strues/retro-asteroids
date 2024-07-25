/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file StringLiteral.hpp
 * @date 2024-07-22
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <algorithm>

namespace ra::utils {

template <std::size_t N>
struct StringLiteral {
  constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, data); }

  char data[N];
  auto operator<=>(const StringLiteral&) const = default;
  bool operator==(const StringLiteral&) const  = default;

  /**
   * @return String's length, not including the null-terminating character.
   */
  inline constexpr std::size_t Size() const { return N - 1U; }
};

}  // namespace ra::utils