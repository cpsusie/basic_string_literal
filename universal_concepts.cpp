//
// Created by cpsusie on 4/25/24.
//
#include "universal_concepts.hpp"

namespace cps::concepts::universal
{
    static_assert(nothrow_convertible_to<bool, const bool>);
}