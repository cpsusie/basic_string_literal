//
// Created by cpsusie on 4/25/24.
//

#ifndef CORE_LITERAL_HPP
#define CORE_LITERAL_HPP
#include "character_concepts.hpp"
#include "universal_concepts.hpp"

namespace cps::core_string_literal
{
    using cps::concepts::core_character::value_t;
    using cps::concepts::core_character::std_arr_sz_v;
    using cps::concepts::core_character::character;
    using cps::concepts::core_character::core_char_array_type;
    using cps::concepts::core_character::same_char_core_char_array_types;
    using cps::std_ext::literals::operator""_szt;
    using cps::concepts::core_character::valid_str_bf_sz;


    namespace detail
    {
        template<core_char_array_type T, T STR_BF>
        struct character_array_data
        {
            using array_type = std::remove_cvref_t<T>;
            using char_type = value_t<array_type>;
            using value_type = char_type;
            using traits_type = std::char_traits<value_type>;
            using size_type = std::remove_cvref_t<typename array_type::size_type>;

            static constexpr array_type array_value = array_type{STR_BF};
            static constexpr size_type bf_sz = std::tuple_size_v<T>;

            using sv_t = std::basic_string_view<char_type, traits_type>;
            using fixed_view_t = std::span<const char_type, bf_sz>;
            using view_t = std::span<const char_type>;
            using ostrm_t = std::basic_ostream<char_type, traits_type>;

            static constexpr auto null_term = char_type{};

            static constexpr char_type buffer_front = array_value.front();
            static constexpr char_type buffer_back = array_value.back();

            static constexpr bool is_back_null_term = buffer_back == null_term;
            static constexpr bool is_front_null_term = buffer_front == null_term;

             static consteval std::size_t count_null_terminators() noexcept
             {
                 return std::ranges::count(array_value, null_term);
             }

             static constexpr std::size_t num_null_terminators = count_null_terminators();
        };
    }

    template<core_char_array_type T, T STR_BF>
    using char_arr_info_t = detail::character_array_data<T, STR_BF>;

    template<typename T>
    concept narrow_string_arr = core_char_array_type<T> && std::same_as<value_t<T>, char>;

    template<typename T>
    concept wide_string_arr = core_char_array_type<T> && std::same_as<value_t<T>, wchar_t>;

    template<typename T>
    concept utf8_string_arr = core_char_array_type<T> && std::same_as<value_t<T>, char8_t>;

    template<typename T>
    concept utf16_string_arr = core_char_array_type<T> && std::same_as<value_t<T>, char16_t>;

    template<typename T>
    concept utf32_string_arr = core_char_array_type<T> && std::same_as<value_t<T>, char32_t>;


    template<typename T, T BUFFER>
    concept valid_str_literal_array = core_char_array_type<T> &&
        char_arr_info_t<T, BUFFER>::is_back_null_term &&
        char_arr_info_t<T, BUFFER>::num_null_terminators == 1_szt &&
        character<typename char_arr_info_t<T, BUFFER>::char_type> &&
        char_arr_info_t<T, BUFFER>::is_front_null_term == (char_arr_info_t<T, BUFFER>::bf_sz == 1_szt);

    template<typename T, T BUFFER>
    concept valid_narrow_lit_array = valid_str_literal_array<T, BUFFER> && narrow_string_arr<T>;

    template<typename T, T BUFFER>
    concept valid_wide_lit_array = valid_str_literal_array<T, BUFFER> && wide_string_arr<T>;

    template<typename T, T BUFFER>
    concept valid_utf8_lit_array = valid_str_literal_array<T, BUFFER> && utf8_string_arr<T>;
    template<typename T, T BUFFER>
    concept valid_utf16_lit_array = valid_str_literal_array<T, BUFFER> && utf16_string_arr<T>;

    template<typename T, T BUFFER>
    concept valid_utf32_lit_array = valid_str_literal_array<T, BUFFER> && utf32_string_arr<T>;

    namespace detail
    {
        template<auto VALUE>
        struct type_from_value
        {
            static constexpr auto sk_value{VALUE};
            using type = std::remove_cvref_t<decltype(sk_value)>;
        };
    }

    template<auto VALUE>
    using type_t = typename detail::type_from_value<VALUE>::type;

    template<auto VALUE>
    concept valid_str_literal_array_value = valid_str_literal_array<type_t<VALUE>, VALUE>;

    template<auto VALUE>
    concept valid_narrow_str_literal_array_value
        = valid_str_literal_array_value<VALUE> && narrow_string_arr<type_t<VALUE>>;


    template<auto VALUE>
    concept valid_wide_str_literal_array_value
        = valid_str_literal_array_value<VALUE> && wide_string_arr<type_t<VALUE>>;

    template<auto VALUE>
    concept valid_utf8_str_literal_array_value
        = valid_str_literal_array_value<VALUE> && utf8_string_arr<type_t<VALUE>>;

    template<auto VALUE>
    concept valid_utf16_str_literal_array_value
        = valid_str_literal_array_value<VALUE> && utf16_string_arr<type_t<VALUE>>;

    template<auto VALUE>
    concept valid_utf32_str_literal_array_value
        = valid_str_literal_array_value<VALUE> && utf32_string_arr<type_t<VALUE>>;

    template<character TChar, std::size_t EXTENT>
        requires (valid_str_bf_sz<EXTENT>)
    consteval bool validate_str_literal(std::span<const TChar, EXTENT> str_literal) noexcept
    {
        constexpr TChar knull_term = TChar{};

        const bool back_is_null_term = str_literal.back() == knull_term;
        const bool front_is_null_term = str_literal.front() == knull_term;

        if constexpr (EXTENT == 1_szt)
        {
            return back_is_null_term && front_is_null_term;
        }
        else
        {
            const std::size_t num_null_term = std::ranges::count(str_literal, knull_term);
            return back_is_null_term && !front_is_null_term && num_null_term == 1_szt;
        }
    }

    template<typename TChar, std::size_t EXTENT>
        requires (character<TChar> && valid_str_bf_sz<EXTENT>)
    struct IMPL_literal final
    {
    private:
        static constexpr std::size_t bf_sz = EXTENT;
        using char_t = std::remove_cvref_t<TChar>;
        using traits_t = std::char_traits<char_t>;
        using array_t = std::array<char_t, bf_sz>;

        using cspan_t = std::span<const char_t, bf_sz>;

        static consteval bool check_valid(cspan_t validate) noexcept
        {
            return validate_str_literal(validate);
        }

    public:
        using array_type = array_t;
        using value_type = char_t;
        using traits_type = traits_t;
        using string_view_type = std::basic_string_view<value_type, traits_type>;

        template<typename TAllocator = std::allocator<value_type>>
        using string_type = std::basic_string<value_type, traits_t, TAllocator>;

        using ostream_type = std::basic_ostream<value_type, traits_type>;

        array_t m_array;

        [[nodiscard]] explicit consteval operator bool() const noexcept
        {
            cspan_t lit = m_array;
            return check_valid(lit);
        }

        [[nodiscard]] consteval bool operator!() const noexcept
        {
            return !static_cast<bool>(*this);
        }

        [[nodiscard]] consteval bool is_valid() const noexcept
        {
            return static_cast<bool>(*this);
        }

        consteval IMPL_literal() noexcept  requires (EXTENT == 1_szt) : m_array{} {}
        constexpr IMPL_literal(const IMPL_literal& other) noexcept = default;
        consteval IMPL_literal(IMPL_literal&& other) noexcept = delete;
        consteval IMPL_literal& operator=(const IMPL_literal& other) noexcept = delete;
        consteval IMPL_literal& operator=(IMPL_literal&& other) noexcept = delete;
        constexpr ~IMPL_literal() noexcept = default;

        constexpr std::strong_ordering operator<=>(const IMPL_literal& other) const noexcept = default;
        constexpr bool operator==(const IMPL_literal& other) const noexcept = default;

        template<std::size_t OExtent>
            requires (valid_str_bf_sz<OExtent> && OExtent != bf_sz)
        constexpr std::strong_ordering operator<=>(const IMPL_literal<value_type, OExtent>& other) const noexcept
        {
            if (std::ranges::lexicographical_compare(m_array, other.m_array))
            {
                return std::strong_ordering::less;
            }
            if (std::ranges::lexicographical_compare(other.m_array, m_array))
            {
                return std::strong_ordering::greater;
            }
            return std::strong_ordering::equal;
        }

        template<std::size_t OExtent>
            requires (valid_str_bf_sz<OExtent> && OExtent != bf_sz)
        constexpr bool operator==(const IMPL_literal<value_type, OExtent>& other) const noexcept
        {
            return false;
        }

        consteval IMPL_literal(const char_t(&arr)[EXTENT]) : m_array{} // NOLINT(*-explicit-constructor)
        {
            std::ranges::copy_n(std::begin(arr), EXTENT, std::begin(m_array));
            cspan_t span = m_array;
            std_ext::consteval_assert(check_valid(span), "Invalid null terminator placement in buffer.");
        }
    };

    template<std::size_t EXTENT>
        requires (EXTENT > 0_szt)
    IMPL_literal(const char(&literal)[EXTENT])->IMPL_literal<char, EXTENT>;

    template<std::size_t EXTENT>
        requires (EXTENT > 0_szt)
    IMPL_literal(const wchar_t(&literal)[EXTENT])->IMPL_literal<wchar_t, EXTENT>;

    template<std::size_t EXTENT>
        requires (EXTENT > 0_szt)
    IMPL_literal(const char8_t(&literal)[EXTENT])->IMPL_literal<char8_t, EXTENT>;

    template<std::size_t EXTENT>
        requires (EXTENT > 0_szt)
    IMPL_literal(const char16_t(&literal)[EXTENT])->IMPL_literal<char16_t, EXTENT>;

    template<std::size_t EXTENT>
    IMPL_literal(const char32_t(&literal)[EXTENT]) -> IMPL_literal<char32_t, EXTENT>;

    namespace IMPL_literal_traits
    {
        template<typename T>
        struct is_impl_literal : std::false_type{};

        template<character TChar, std::size_t EXTENT>
            requires (valid_str_bf_sz<EXTENT>)
        struct is_impl_literal<IMPL_literal<TChar, EXTENT>> : std::true_type{};

        template<typename T>
        constexpr bool is_impl_literal_v = is_impl_literal<T>::value;
    }

    template<typename TImplLit>
    concept IMPL_string_literal_type = IMPL_literal_traits::is_impl_literal_v<TImplLit>;

    template<typename TLhs, typename TRhs>
    concept IMPL_same_char_literal_types =
            IMPL_string_literal_type<TLhs> &&
            IMPL_string_literal_type<TRhs> &&
            concepts::core_character::same_as_without_cvref<typename TLhs::value_type, typename TRhs::value_type>;

    template<typename TLhs, typename TRhs>
    concept IMPL_same_char_distinct_literal_types =
            IMPL_same_char_literal_types<TLhs, TRhs> &&
            !concepts::core_character::same_as_without_cvref<TLhs, TRhs>;


    namespace IMPL_literals
    {
        template<IMPL_literal LITERAL_VALUE>
        struct IMPL_literal_factory
        {
            static constexpr auto literal_value = IMPL_literal{LITERAL_VALUE};
            static_assert(literal_value.is_valid(), "Literal value has improper null-terminators.");

            using array_t = std::remove_cvref_t<decltype(literal_value.m_array)>;
            using char_type = std::remove_cvref_t<std::tuple_element_t<0_szt, array_t>>;
            static constexpr std::size_t bf_sz = std::tuple_size_v<array_t>;
            using IMPL_literal_type = IMPL_literal<char_type, bf_sz>;


            //todo remove from release
            static_assert(std::same_as<array_t, std::array<char_type, bf_sz>> &&
                concepts::core_character::same_as_without_cvref<decltype(literal_value), IMPL_literal_type>);

            consteval IMPL_literal_factory() noexcept = default;
            IMPL_literal_factory(const IMPL_literal_factory& other) noexcept = delete;
            IMPL_literal_factory(IMPL_literal_factory&& other) noexcept = delete;
            IMPL_literal_factory& operator=(const IMPL_literal_factory& other) noexcept = delete;
            IMPL_literal_factory& operator=(IMPL_literal_factory&& other) noexcept =delete;
            constexpr ~IMPL_literal_factory() noexcept = default;

            constexpr auto operator()() const noexcept -> IMPL_literal_type
            {
                return literal_value;
            }

        };

        template<IMPL_literal LITERAL_VALUE>
        using char_t = typename IMPL_literal_factory<LITERAL_VALUE>::char_type;

        template<IMPL_literal LITERAL_VALUE>
        constexpr std::size_t buffer_extent_v = IMPL_literal_factory<LITERAL_VALUE>::bf_sz;

        template<IMPL_literal LITERAL_VALUE>
        using IMPL_literal_t = typename IMPL_literal_factory<LITERAL_VALUE>::IMPL_literal_type;

        template<IMPL_literal LITERAL_VALUE>
        consteval auto operator""_IMPL_lit() noexcept -> IMPL_literal_t<LITERAL_VALUE>
        {
            constexpr auto factory = IMPL_literal_factory<LITERAL_VALUE>{};
            return factory();
        }
    }


}



#endif //CORE_LITERAL_HPP
