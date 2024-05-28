#ifndef CPS_CTIME_SV_INTERNAL_CONCEPTS_CORE_CHARACTER_HPP
#define CPS_CTIME_SV_INTERNAL_CONCEPTS_CORE_CHARACTER_HPP
#include "std_includes.hpp"
#include "std_ext.hpp"
namespace cps::concepts::core_character
{
	using std_ext::literals::operator ""_szt;
	namespace internal
	{

		template<typename T>
		using remove_vref_t = std::remove_reference_t<std::remove_volatile_t<T>>;
	}

	template<typename TFirst, typename TSecond>
	concept same_as_without_cvref = std::same_as<std::remove_cvref_t<TFirst>, std::remove_cvref_t<TSecond>>;

	template<typename TFirst, typename TSecond>
	concept same_as_without_vref = std::same_as<internal::remove_vref_t<TFirst>, internal::remove_vref_t<TSecond>>;

	template<typename TFirst, typename TSecond>
	concept same_as_without_ref = std::same_as<std::remove_reference_t<TFirst>, std::remove_reference_t<TSecond>>;

	template<typename T>
	concept narrow_mutable_character = same_as_without_vref<T, char>;

	template<typename T>
	concept narrow_const_character = same_as_without_vref<T, const char>;

	template<typename T>
	concept narrow_character = narrow_mutable_character<T> || narrow_const_character<T>;


	template<typename T>
	concept wide_mutable_character = same_as_without_vref<T, wchar_t>;

	template<typename T>
	concept wide_const_character = same_as_without_vref<T, const wchar_t>;

	template<typename T>
	concept wide_character = wide_mutable_character<T> || wide_const_character<T>;


	template<typename T>
	concept mutable_console_character = narrow_mutable_character<T> || wide_mutable_character<T>;

	template<typename T>
	concept const_console_character = narrow_const_character<T> || wide_const_character<T>;

	template<typename T>
	concept console_character = mutable_console_character<T> || const_console_character<T>;


	template<typename T>
	concept utf8_mutable_character = same_as_without_vref<T, char8_t>;

	template<typename T>
	concept utf8_const_character = same_as_without_vref<T, const char8_t>;

	template<typename T>
	concept utf8_character = utf8_mutable_character<T> || utf8_const_character<T>;

	template<typename T>
	concept utf16_mutable_character = same_as_without_vref<T, char16_t>;

	template<typename T>
	concept utf16_const_character = same_as_without_vref<T, const char16_t>;

	template<typename T>
	concept utf16_character = utf16_mutable_character<T> || utf16_const_character<T>;

	template<typename T>
	concept utf32_mutable_character = same_as_without_vref<T, char32_t>;

	template<typename T>
	concept utf32_const_character = same_as_without_vref<T, const char32_t>;

	template<typename T>
	concept utf32_character = utf32_mutable_character<T> || utf32_const_character<T>;


	template<typename T>
	concept mutable_utf_character
		= utf8_mutable_character<T> || utf16_mutable_character<T> || utf32_mutable_character<T>;

	template<typename T>
	concept const_utf_character
		= utf8_const_character<T> || utf16_const_character<T> || utf32_const_character<T>;

	template<typename T>
	concept utf_character = mutable_utf_character<T> || const_utf_character<T>;

	template<typename T>
	concept mutable_character = mutable_console_character<T> || mutable_utf_character<T>;

	template<typename T>
	concept const_character = const_console_character<T> || const_utf_character<T>;

	template<typename T>
	concept character = mutable_character<T> || const_character<T>;

	constexpr std::size_t bits_per_narrow_char = CHAR_BIT;

	namespace traits
	{
		template<character T>
		struct character_traits
		{
			using char_t = std::remove_cvref_t<T>;
			using const_char_t = std::add_const_t<char_t>;
			using traits_t = std::char_traits<char_t>;
			using limits_t = std::numeric_limits<char_t>;

			static constexpr std::size_t char_size = sizeof(char_t);
			static constexpr bool is_signed = limits_t::is_signed;
			static constexpr int binary_magnitude_digits = limits_t::digits;
			static constexpr int sign_digits = is_signed ? binary_magnitude_digits + 1 : 0;
			static constexpr int total_digits = sign_digits + binary_magnitude_digits;
			
		};
	}

	template<character T>
	using char_traits_t = typename traits::character_traits<T>::traits_t;

    template<typename TTraits>
    using traits_char_t = std::remove_cvref_t<typename TTraits::char_type>;

	template<character T>
	constexpr bool is_signed_v = traits::character_traits<T>::is_signed;

	template<character T>
	constexpr int binary_magnitude_digits_v = traits::character_traits<T>::binary_magnitude_digits;

	template<character T>
	constexpr int binary_sign_digits = traits::character_traits<T>::sign_digits;

	template<character T>
	constexpr int total_binary_digits = traits::character_traits<T>::total_digits;


	template<typename TTraits>
	concept standard_char_trait =
		same_as_without_cvref<TTraits, std::char_traits<char>>		||
		same_as_without_cvref<TTraits, std::char_traits<wchar_t>>	||
		same_as_without_cvref<TTraits, std::char_traits<char8_t>>	||
		same_as_without_cvref<TTraits, std::char_traits<char16_t>>	||
		same_as_without_cvref<TTraits, std::char_traits<char32_t>>;

	template<typename TChar, typename TTraits>
	concept char_matching_traits = character<TChar> && standard_char_trait<TTraits>
            && same_as_without_cvref<traits_char_t<TTraits>, TChar>;

	template<std::size_t STR_BF_SZ>
	concept valid_str_bf_sz = STR_BF_SZ != 0_szt && STR_BF_SZ != std::dynamic_extent;

	namespace traits
	{
		namespace detail
		{
			template<typename TArr>
			struct is_std_array : public std::false_type{};

			template<typename TVal, std::size_t EXTENT>
			struct is_std_array<std::array<TVal, EXTENT>> : public std::true_type{};
		}

		template<typename TMaybeStdArray>
		static constexpr bool is_std_array_v = detail::is_std_array<TMaybeStdArray>::value;
	}

	template<typename T>
	concept std_array = traits::is_std_array_v<T>;

	namespace traits
	{
		namespace detail
		{
			template<std_array T>
			struct std_array_info final
			{
				using array_type = std::remove_cvref_t<T>;
				static constexpr std::size_t array_sz = std::tuple_size_v<array_type>;
				using value_type = typename array_type::value_type;

				static_assert(std::same_as<array_type, std::array<value_type, array_sz>>);
			};
		}

		template<std_array T>
		static constexpr std::size_t std_arr_sz_v = detail::std_array_info<T>::array_sz;

		template<std_array T>
		using value_t = typename detail::std_array_info<T>::value_type;
	}

	using traits::std_arr_sz_v;
	using traits::value_t;

	template<typename TCharArr>
	concept core_char_array_type = std_array<TCharArr> && character<value_t<TCharArr>> && valid_str_bf_sz<std_arr_sz_v<TCharArr>>;

	template<typename TCharArrL, typename TCharArrR>
	concept same_char_core_char_array_types = core_char_array_type<TCharArrL> && core_char_array_type<TCharArrR> && same_as_without_cvref<value_t<TCharArrL>, value_t<TCharArrR>>;
}



#endif // CPS_CTIME_SV_INTERNAL_CONCEPTS_CORE_CHARACTER_HPP