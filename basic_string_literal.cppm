module;
#include "std_includes.hpp"
#include "std_ext.hpp"
#include "character_concepts.hpp"
#include "universal_concepts.hpp"
export module basic_string_literal;

static constexpr auto newl = '\n';

namespace cps::bsl_lib
{
    export void PrintGreeting(std::string_view message);

    static void try_out_bsl();

}

namespace cps::bsl_lib
{
    namespace UNCON = cps::concepts::universal;
    namespace CHRCON = cps::concepts::core_character;

    export template<typename TSrc, typename TDst>
    concept nothrow_convertible_to = UNCON::nothrow_convertible_to<TSrc, TDst>;

    export template<typename TSrc, typename TDst>
    concept static_castable_to = UNCON::static_castable_to<TSrc, TDst>;

    export template<typename TFirst, typename TSecond>
    concept same_as_without_cvref = CHRCON::same_as_without_cvref<TFirst, TSecond>;

    export template<typename T>
    concept narrow_character = CHRCON::narrow_character<T>;

    export template<typename T>
    concept wide_character = CHRCON::wide_character<T>;

    export template<typename T>
    concept utf8_character = CHRCON::utf8_character<T>;

    export template<typename T>
    concept utf16_character = CHRCON::utf16_character<T>;

    export template<typename T>
    concept utf32_character = CHRCON::utf32_character<T>;

    export template<typename T>
    concept utf_character = CHRCON::utf_character<T>;

    export template<typename T>
    concept console_character = CHRCON::console_character<T>;

    export template<typename T>
    concept character = CHRCON::character<T>;

    export constexpr std::size_t bits_per_narrow_char = CHRCON::bits_per_narrow_char;

    export template<typename TTraits>
    concept standard_char_trait = CHRCON::standard_char_trait<TTraits>;

    export template<typename TChar, typename TTraits>
    concept char_matching_traits = CHRCON::char_matching_traits<TChar, TTraits>;

    export template<std::size_t STR_BF_SZ>
    concept valid_str_bf_sz = CHRCON::valid_str_bf_sz<STR_BF_SZ>;
}

namespace cps::bsl_lib::internal
{
    export using std_ext::literals::operator""_szt;

    namespace detail
    {
        template<typename TCompCat>
        concept is_comparison_category =
            same_as_without_cvref<TCompCat, std::strong_ordering> ||
            same_as_without_cvref<TCompCat, std::weak_ordering> ||
            same_as_without_cvref<TCompCat, std::partial_ordering>;

        template<typename TCompCat>
        struct greater_and_lesser_values;

        template<>
        struct greater_and_lesser_values<std::strong_ordering>
        {
            using comparison_category = std::strong_ordering;
            static constexpr comparison_category greater = std::strong_ordering::greater;
            static constexpr comparison_category lesser = std::strong_ordering::less;
        };

        template<>
        struct greater_and_lesser_values<std::weak_ordering>
        {
            using comparison_category = std::weak_ordering;
            static constexpr comparison_category greater = std::weak_ordering::greater;
            static constexpr comparison_category lesser = std::weak_ordering::less;
        };

        template<>
        struct greater_and_lesser_values<std::partial_ordering>
        {
            using comparison_category = std::partial_ordering;
            static constexpr comparison_category greater = std::partial_ordering::greater;
            static constexpr comparison_category lesser = std::partial_ordering::less;
        };

        template<is_comparison_category TCategory>
        using comp_category_t = typename greater_and_lesser_values<TCategory>::comparison_category;

        template<is_comparison_category TCategory>
        constexpr comp_category_t<TCategory> greater_than_v = greater_and_lesser_values<TCategory>::greater;

        template<is_comparison_category TCategory>
        constexpr comp_category_t<TCategory> less_than_v = greater_and_lesser_values<TCategory>::lesser;
    }

    using detail::is_comparison_category;
    using detail::comp_category_t;
    using detail::greater_than_v;
    using detail::less_than_v;

    export template<character TChar, std::size_t EXTENT>
        requires (valid_str_bf_sz<EXTENT>)
    struct internal_string_literal
    {
        using character_type = std::remove_cvref_t<TChar>;
        using character_traits_type = std::char_traits<character_type>;
        using comparison_category_type = typename character_traits_type::comparison_category;

        static constexpr std::size_t bf_sz = EXTENT;

        using mspan_type = std::span<character_type, bf_sz>;
        using cspan_type = std::span<std::add_const_t<character_type>, bf_sz>;

        using std_array_type = std::array<character_type, EXTENT>;

        std_array_type m_buffer;

        consteval internal_string_literal() noexcept requires (bf_sz == 1_szt) = default;
        consteval internal_string_literal(const character_type(&arr)[bf_sz]) noexcept : m_buffer{}
        {
            std::ranges::copy_n(std::begin(arr), bf_sz, std::begin(m_buffer) );
        }
        constexpr internal_string_literal(const internal_string_literal& other) noexcept = default;
        consteval internal_string_literal(internal_string_literal&& other) noexcept = default;
        consteval internal_string_literal& operator=(const internal_string_literal& other) noexcept = default;
        consteval internal_string_literal& operator=(internal_string_literal&& other) noexcept = default;

        constexpr bool operator==(const internal_string_literal& other) const noexcept = default;
        constexpr auto operator<=>(const internal_string_literal& other) const noexcept
            -> comparison_category_type = default;

        template<std::size_t OTHER_EXT>
            requires (valid_str_bf_sz<OTHER_EXT> && OTHER_EXT != bf_sz)
        constexpr bool operator==(const internal_string_literal<character_type, OTHER_EXT>& other) const noexcept
        {
            return false;
        }

        template<std::size_t OTHER_EXT>
            requires (valid_str_bf_sz<OTHER_EXT> && OTHER_EXT != bf_sz)
        constexpr auto operator<=>(const internal_string_literal<character_type, OTHER_EXT>& other) const noexcept
        {
            return std::ranges::lexicographical_compare(m_buffer, other.m_buffer) ?
                less_than_v<comparison_category_type> : greater_than_v<comparison_category_type>;
        }

        consteval auto get_span() noexcept -> mspan_type
        {
            return m_buffer;
        }

        constexpr auto get_span() const noexcept -> cspan_type
        {
            return m_buffer;
        }

    };

    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    internal_string_literal(const char(&arr)[EXTENT]) -> internal_string_literal<char, EXTENT>;

    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    internal_string_literal(const wchar_t(&arr)[EXTENT]) -> internal_string_literal<wchar_t, EXTENT>;

    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    internal_string_literal(const char8_t(&arr)[EXTENT]) -> internal_string_literal<char8_t, EXTENT>;

    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    internal_string_literal(const char16_t(&arr)[EXTENT]) -> internal_string_literal<char16_t, EXTENT>;

    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    internal_string_literal(char32_t(&arr)[EXTENT]) -> internal_string_literal<char32_t, EXTENT>;

    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    using narrow_internal_literal = internal_string_literal<char, EXTENT>;


    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    using wide_internal_literal = internal_string_literal<wchar_t, EXTENT>;

    export template<std::size_t EXTENT>
        requires valid_str_bf_sz<EXTENT>
    using utf8_internal_literal = internal_string_literal<char8_t, EXTENT>;

    export template<std::size_t EXTENT>
            requires valid_str_bf_sz<EXTENT>
    using utf16_internal_literal = internal_string_literal<char16_t, EXTENT>;

    export template<std::size_t EXTENT>
            requires valid_str_bf_sz<EXTENT>
    using utf32_internal_literal = internal_string_literal<char32_t, EXTENT>;

    namespace detail
    {
        template<typename T>
        struct is_internal_string_literal : public std::false_type {};

        template<character TChar, std::size_t EXTENT>
            requires valid_str_bf_sz<EXTENT>
        struct is_internal_string_literal<internal_string_literal<TChar, EXTENT>> : public std::true_type {};

        template<typename T>
        concept isl_type = is_internal_string_literal<T>{}();
    }

    export template<typename T>
    concept internal_lit_type = detail::isl_type<T>;

    export template<internal_lit_type T>
    using char_t = std::remove_cvref_t<typename T::character_type>;

    export template<internal_lit_type T>
    constexpr std::size_t buff_size_v = T::bf_sz;

    export template<internal_lit_type T>
    using array_t = std::remove_cvref_t<typename T::std_array_type>;

    export template<typename T, typename TChar>
    concept internal_literal_of = character<TChar> && internal_lit_type<T> && requires
    {
        same_as_without_cvref<TChar, char_t<T>>;
    };

    export template<typename T>
    concept narrow_internal_lit = internal_literal_of<T, char>;

    export template<typename T>
    concept wide_internal_lit = internal_literal_of<T, wchar_t>;

    export template<typename T>
    concept utf8_internal_lit = internal_literal_of<T, char8_t>;

    export template<typename T>
    concept utf16_internal_lit = internal_literal_of<T, char16_t>;

    export template<typename T>
    concept utf32_internal_lit = internal_literal_of<T, char8_t>;

    export template<typename T, std::size_t EXTENT>
    concept internal_lit_of_extent = internal_lit_type<T> && valid_str_bf_sz<EXTENT> && buff_size_v<T> == EXTENT;

    export template<typename TStrLitOne, typename TStrLitTwo>
    concept compatible_internal_literals = internal_lit_type<TStrLitOne> && internal_lit_type<TStrLitTwo> && requires
    {
        same_as_without_cvref<char_t<TStrLitOne>, char_t<TStrLitTwo>>;
    };

    export template<typename TStrLitOne, typename TStrLitTwo>
    concept distinct_typed_compatible_literals = compatible_internal_literals<TStrLitOne, TStrLitTwo> &&
            !same_as_without_cvref<TStrLitOne, TStrLitTwo>;
}

namespace cps::bsl_lib
{
    using internal::internal_string_literal;
    export using std_ext::literals::operator""_szt;

    export template<character TChar, std::size_t BF_SZ>
        requires valid_str_bf_sz<BF_SZ>
    class basic_string_literal_view;

    export template<internal_string_literal LITERAL>
    class basic_string_literal;

    export using internal::internal_string_literal;

    namespace literals
    {
        template<internal_string_literal LITERAL>
        struct factory;

        template<internal_string_literal LITERAL>
        using bsl_t = typename factory<LITERAL>::bsl_t;

        export template<internal_string_literal LITERAL>
        consteval auto operator""_bsl() noexcept -> bsl_t<LITERAL>;

    }

    namespace traits
    {
        template<internal_string_literal LITERAL>
        class isl_value_and_type_data
        {
            static constexpr auto literal_value = internal_string_literal{LITERAL};
            using internal_literal_type  = std::remove_cvref_t<decltype(literal_value)>;
            using array_t = std::remove_cvref_t<decltype(literal_value.m_buffer)>;
            using char_t = std::tuple_element_t<0_szt, array_t>;
            using traits_t = std::char_traits<char_t>;
            static constexpr std::size_t bf_sz = std::tuple_size_v<array_t>;
            static constexpr std::span<const char_t, bf_sz> array_view = literal_value.m_buffer;

        public:
            using value_type = char_t;
            using traits_type = traits_t;
            using std_sv_type = std::basic_string_view<value_type, traits_type>;
            using std_ostr_type = std::basic_ostream<value_type,traits_type>;
            using array_type = std::array<value_type, bf_sz>;
            static_assert(std::same_as<array_type, array_t>);
            using array_view_type = std::span<const char_t, bf_sz>;
            using internal_string_literal_type = internal_string_literal<value_type, bf_sz>;
            using basic_string_literal_type = basic_string_literal<literal_value>;
            using basic_string_literal_view_type = basic_string_literal_view<value_type, bf_sz>;

            static constexpr auto as_string_view = std_sv_type{literal_value.m_buffer.data()};

            static constexpr std::size_t buffer_extent = bf_sz;

            template<typename TAllocator = std::allocator<value_type>>
            using std_string_type = std::basic_string<value_type, traits_type, TAllocator>;

            template<internal_string_literal OTH_LITERAL>
            friend class basic_string_literal;

            template<character TChar, std::size_t BF_SZ>
                requires valid_str_bf_sz<BF_SZ>
            friend class basic_string_literal_view;

            static constexpr auto get_array() noexcept -> array_type
            {
                return literal_value.m_buffer;
            }

            static constexpr auto get_array_view() noexcept -> array_view_type
            {
                return array_view;
            }

            template<typename TAllocator = std::allocator<value_type>>
            static constexpr auto get_std_string() -> std_string_type<TAllocator>
            {
                return static_cast<std_string_type<TAllocator>>(as_string_view);
            }

        };
    }

    template<character TChar, std::size_t BF_SZ>
        requires valid_str_bf_sz<BF_SZ>
    class basic_string_literal_view
    {
       static constexpr std::size_t bf_sz = BF_SZ;
       using char_t = std::remove_cvref_t<TChar>;
       using traits_t = std::char_traits<char_t>;
       using isl_t = internal_string_literal<char_t, bf_sz>;
       using arr_t = std::array<char_t, bf_sz>;
       using cspan_t = std::span<const char_t, bf_sz>;
       using sv_t = std::basic_string_view<char_t, traits_t>;
       using ostr_t = std::basic_ostream<char_t, traits_t>;

    public:
        using value_type = std::remove_cvref_t<TChar>;
        using traits_type = std::char_traits<value_type>;
        using size_type = typename arr_t::size_type;
        using isl_type = isl_t;
        using ostr_type = ostr_t;

        template<internal_string_literal LITERAL_VALUE>
        friend class traits::isl_value_and_type_data;

        friend auto operator<<(ostr_type& os, basic_string_literal_view v) -> ostr_type&
        {
            os << v.get_sv();
            return os;
        }

    private:

        constexpr auto get_sv() const noexcept -> sv_t
        {
            return sv_t{m_lit_view.data()};
        }

        cspan_t m_lit_view;
    };

    export template<internal_string_literal LITERAL>
    class basic_string_literal
    {
        using tv_info_t = traits::isl_value_and_type_data<LITERAL>;
        using char_t = typename tv_info_t::value_type;
        static constexpr std::size_t bf_sz = tv_info_t::buffer_extent;
        using bsl_view_t = basic_string_literal_view<char_t, bf_sz>;
        using isl_t = typename tv_info_t::internal_string_literal_type;

    public:

        template<internal_string_literal OTH_LITERAL>
        friend class basic_string_literal;

        template<internal_string_literal OTH_LITERAL>
        friend class literals::factory;

        using value_type = char_t;
        using traits_type = std::char_traits<value_type>;
        using bsl_view_type = bsl_view_t;
        using std_sv_type = typename tv_info_t::std_sv_type;
        using array_type = typename tv_info_t::array_type;
        using array_view_type = typename tv_info_t::array_view_type;
        using ostream_type = typename tv_info_t::std_ostr_type;
        template<typename TAllocator = std::allocator<value_type>>
        using string_type = typename tv_info_t::template std_string_type<TAllocator>;


        constexpr basic_string_literal(const basic_string_literal& other) = default;
        constexpr ~basic_string_literal() noexcept = default;
        consteval basic_string_literal(basic_string_literal&& other) noexcept = delete;
        constexpr basic_string_literal& operator=(const basic_string_literal&) noexcept = delete;
        constexpr basic_string_literal& operator=(basic_string_literal&& ) noexcept = delete;


        template<typename TAllocator = std::allocator<value_type>>
        [[nodiscard]] constexpr explicit operator string_type<TAllocator>() const
        {
            return tv_info_t::template get_std_string<TAllocator>();
        }

        template<typename TAllocator = std::allocator<value_type>>
        [[nodiscard]] constexpr auto to_string() const -> string_type<TAllocator>
        {
            return static_cast<string_type<TAllocator>>(*this);
        }

        [[nodiscard]] consteval explicit operator std_sv_type() const noexcept
        {
            return tv_info_t::as_string_view;
        }

        [[nodiscard]] consteval auto to_std_sv() const noexcept -> std_sv_type
        {
            return static_cast<std_sv_type>(*this);
        }

        [[nodiscard]] constexpr auto to_std_array() const noexcept -> array_type
        {
            return tv_info_t::get_array();
        }

        [[nodiscard]] constexpr auto to_array_view() const noexcept -> array_view_type
        {
            return tv_info_t::get_array_view();
        }

        constexpr bool operator==(const basic_string_literal& other) const noexcept
        {
            return true;
        }

        constexpr std::strong_ordering operator<=>(const basic_string_literal& other) const noexcept
        {
            return std::strong_ordering::equal;
        }

        template<typename TOtherLiteral>
            requires (internal::distinct_typed_compatible_literals<basic_string_literal, TOtherLiteral>)
        constexpr bool operator==(const TOtherLiteral& other) const noexcept
        {
            return static_cast<std_sv_type>(*this) == static_cast<std_sv_type>(other);
        }

        template<typename TOtherLiteral>
            requires (internal::distinct_typed_compatible_literals<basic_string_literal, TOtherLiteral>)
        constexpr std::strong_ordering operator<=>(const TOtherLiteral& other) const noexcept
        {
            return static_cast<std_sv_type>(*this) <=> static_cast<std_sv_type>(other);
        }



        friend auto operator<<(ostream_type& os, basic_string_literal literal)
        {
            os << literal.to_std_sv();
        }

    private:
        consteval basic_string_literal() noexcept = default;

    };



    namespace literals
    {

        export using internal::narrow_internal_literal;
        export using internal::wide_internal_literal;
        export using internal::utf8_internal_literal;
        export using internal::utf16_internal_literal;
        export using internal::utf32_internal_literal;

        template<internal_string_literal LITERAL>
        struct factory_core
        {
            static constexpr auto value = internal_string_literal{LITERAL};
            using literal_type = std::remove_cvref_t<decltype(value)>;
            using char_type = std::remove_cvref_t<typename literal_type::character_type>;
            static constexpr std::size_t bf_sz = literal_type::bf_sz;
        };

        template<internal_string_literal LITERAL>
        struct factory
        {
            using core_type = factory_core<LITERAL>;
            using char_type = typename core_type::char_type;
            static constexpr std::size_t bf_sz = core_type::bf_sz;
            using isl_t = internal_string_literal<char_type, bf_sz>;
            static constexpr auto literal_value = isl_t{LITERAL};
            using bsl_t= basic_string_literal<literal_value>;

            consteval factory() noexcept = default;

            constexpr auto operator()() const noexcept -> bsl_t
            {
                return bsl_t{};
            }
        };

        export template<internal_string_literal LITERAL>
        consteval auto operator""_bsl() noexcept -> bsl_t<LITERAL>
        {
            constexpr auto fact = factory<LITERAL>{};
            return fact();
        }



    }
}

void cps::bsl_lib::PrintGreeting(std::string_view message)
{
    using cps::bsl_lib::literals::operator""_bsl;
    std::cout << "Greeting message: " << std::quoted(message) << "." << newl;
    std::cout << "Here is my first string literal: " << std::quoted("Hi mom!"_bsl.to_std_sv()) << "." << newl;
}

void cps::bsl_lib::try_out_bsl()
{
     using namespace cps::bsl_lib::literals;

     constexpr auto hi = "Hi mom!"_bsl;
     std::cout << "Hello msg: " << hi.to_std_sv() << ".";
}
