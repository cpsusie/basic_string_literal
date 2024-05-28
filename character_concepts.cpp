#include "character_concepts.hpp"

namespace cps::concepts::core_character
{
    static_assert(character<char> && character<const wchar_t> && !character<int>);
    using ca_1_t = std::array<char, 0_szt>;
    using ca_2_t = std::array<int, 1_szt>;

    using ca_3_t = std::array<char8_t, 12_szt>;
    using ca_4_t = std::array<char8_t, 1_szt>;
    using ca_5_t = std::array<char8_t, 0_szt>;

    static_assert(core_char_array_type<ca_3_t>);

    static_assert(!core_char_array_type<ca_2_t>);
    static_assert(!core_char_array_type<ca_1_t>);

    static_assert(same_char_core_char_array_types<ca_3_t, ca_4_t>);
    static_assert(!same_char_core_char_array_types<ca_3_t, ca_5_t>);


}