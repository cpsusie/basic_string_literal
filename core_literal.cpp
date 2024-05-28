#include "core_literal.hpp"
namespace cps::core_string_literal
{
    static constexpr auto hi_mom = std::array{'h', 'i',' ', 'm', 'o', 'm', '\0'};
    using hi_mom_t = std::remove_cvref_t<decltype(hi_mom)>;
    static_assert(core_char_array_type<hi_mom_t>);

    static constexpr auto bye_mom = std::array{u8'\0'};
    using bye_mom_t = std::remove_cvref_t<decltype(bye_mom)>;

    static_assert(valid_str_literal_array<hi_mom_t, hi_mom>);
    static_assert(valid_str_literal_array<bye_mom_t, bye_mom>);

    static_assert(std::same_as<type_t<hi_mom>, hi_mom_t>);
    static_assert(valid_str_literal_array_value<bye_mom>);
    static_assert(valid_str_literal_array_value<hi_mom>);

    static_assert(valid_narrow_str_literal_array_value<hi_mom>);
    static_assert(!valid_narrow_str_literal_array_value<bye_mom>);

    static_assert(valid_utf8_str_literal_array_value<bye_mom>);
    static_assert(!valid_utf8_str_literal_array_value<hi_mom>);

    using IMPL_literals::operator ""_IMPL_lit;

    constexpr auto hi_mom_lit = "Hi mom!"_IMPL_lit;
    constexpr auto wide_bye_lit = L"Bye mom!"_IMPL_lit;

    constexpr auto u8_spencer_lit = u8"Spencer Cat "_IMPL_lit;
    constexpr auto u16_very_lit = u"is so very, very "_IMPL_lit;
    constexpr auto u32_cute_lit = U"CUTE!!!1eleventy-one!11"_IMPL_lit;

    static_assert(hi_mom_lit.is_valid());
    static_assert(wide_bye_lit.is_valid());

    static_assert(u8_spencer_lit.is_valid());
    static_assert(u16_very_lit.is_valid());
    static_assert(u32_cute_lit.is_valid());

    constexpr auto abercrombie = "Abercrombie"_IMPL_lit;
    constexpr auto benjamin =    "Benjamin"_IMPL_lit;
    constexpr auto christopher = "Christopher"_IMPL_lit;

    constexpr auto empty = ""_IMPL_lit;
    static_assert(empty.is_valid());

    static_assert(std::strong_ordering::equal == (christopher <=> "Christopher"_IMPL_lit));
    static_assert(std::strong_ordering::greater == (christopher <=> benjamin));
    static_assert(std::strong_ordering::less == (abercrombie <=> benjamin));

    static_assert(christopher != abercrombie);
    static_assert(abercrombie < christopher);
    static_assert(benjamin > abercrombie);
    static_assert(benjamin < christopher);

    static constexpr std::strong_ordering empty_vs_a = empty <=> "a"_IMPL_lit;
    static_assert(empty_vs_a == std::strong_ordering::less);

}
