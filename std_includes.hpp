#ifndef STD_INCLUDES_HPP
#define STD_INCLUDES_HPP
#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <concepts>
#include <span>
#include <string>
#include <string_view>
#include <sstream>
#include <array>
#include <iostream>
#include <iomanip>
#include <limits>
#include <bit>
#include <utility>
#include <functional>
#include <optional>
#include <variant>
#include <ranges>
#include <algorithm>
#include <compare>
#include <memory>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <iomanip>
#include <tuple>

namespace cps::string_literal
{
    using std::string_view_literals::operator""sv;
    using std::string_literals::operator""s;

    static constexpr auto newl = '\n';
    static constexpr auto wnewl = L'\n';
    static constexpr auto u8newl = u8'\n';
    static constexpr auto u16newl = u'\n';
    static constexpr auto u32newl = U'\n';
}


#endif //STD_INCLUDES_HPP
