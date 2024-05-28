#ifndef CPS_CTIME_SV_STD_EXT_HPP
#define CPS_CTIME_SV_STD_EXT_HPP
#include "std_includes.hpp"

namespace cps::std_ext::internal
{
	struct not_there_tag {};

	using size_t_proxy_t = std::uint32_t;

	template<typename TEnum>
	concept is_enum = std::is_enum_v<TEnum>;

	template<typename TSrc, typename TDst>
	concept nothrow_convertible_to = std::convertible_to<TSrc, TDst> && std::is_nothrow_convertible_v<TSrc, TDst>;

	template<is_enum TEnum>
	struct to_underlying_proxy
	{
		using enum_t = std::remove_cvref_t<TEnum>;
		using under_t = std::underlying_type_t<TEnum>;
		constexpr auto operator()(TEnum val) const noexcept -> under_t;
	};

	[[noreturn]] inline void unreachable_impl()
	{
#ifdef __GNUC__ // GCC, Clang, ICC
		__builtin_unreachable();
#elif defined _MSC_VER // MSVC
		__assume(false);
#endif
	}

	[[noreturn]] inline void consteval_fail(std::string_view msg)
	{
#ifndef _NDEBUG
		std::cerr << msg << "\n";
		throw std::logic_error{ "Assertion failed" };  // NOLINT(clang-diagnostic-exceptions)
#endif
		unreachable_impl();
	}

	namespace detail
	{
		template<auto LITERAL_VALUE_TYPE>
		struct literal_value_info_core
		{
			static constexpr auto literal_value = LITERAL_VALUE_TYPE;
			using literal_value_type_type = std::remove_const_t<decltype(literal_value)>;
		};

		template<auto LITERAL_VALUE>
		using literal_value_type_t = typename literal_value_info_core<LITERAL_VALUE>::literal_value_type_type;

		template<auto LITERAL_VALUE>
		constexpr literal_value_type_t<LITERAL_VALUE> literal_value_type_v = LITERAL_VALUE;

		namespace detail
		{

			template<typename T, T FIRST, T SECOND>
			struct equality_test_core
			{
				static constexpr literal_value_type_t<FIRST> first_value = FIRST;
				static constexpr literal_value_type_t<SECOND> second_value = SECOND;

				consteval bool operator()() const noexcept { return first_value == second_value; }
			};
		}

		template<typename TValType, TValType LITERAL_VALUE_ONE, TValType LITERAL_VALUE_TWO>
		consteval bool check_equals() noexcept
		{
			return detail::equality_test_core<TValType, LITERAL_VALUE_ONE, LITERAL_VALUE_TWO>{}();
		}

		template<auto LITERAL_VALUE_ONE>
		concept is_valid_literal_value = requires (const literal_value_type_t<LITERAL_VALUE_ONE>& value_one)
		{
			std::equality_comparable<literal_value_type_t<LITERAL_VALUE_ONE>>;
			check_equals <literal_value_type_t<LITERAL_VALUE_ONE>, LITERAL_VALUE_ONE, literal_value_type_v<LITERAL_VALUE_ONE> > ();
		};


		constexpr auto test = std::array{ 1, 2, 3 };

		static_assert(is_valid_literal_value<'1'>);
		static_assert(is_valid_literal_value<test>);

		template<int VALUE_ONE, int VALUE_TWO>
		struct consteval_adder
		{
			consteval int operator()() const noexcept
			{
				return VALUE_ONE + VALUE_TWO;
			}
		};



		template<std::size_t N, std::size_t O, std::array<int, N> FIRST, std::array<int, O> SECOND>
		struct are_arrays_equal
		{
			static constexpr std::array<int, N> first_array = FIRST;
			static constexpr std::array<int, O> second_array = SECOND;
			consteval bool operator()() const noexcept
			{

				return std::ranges::equal(first_array, second_array);
			}
		};

		//static_assert(std::lexicographical_compare(std::array{-2, }))

		static constexpr auto equal = are_arrays_equal < 3u, 3u, std::array{ -2, -1, 0 }, std::array{ -3 + 1, -2 + 1, -1 + 1 } > {}();
		static constexpr auto not_equal_different_lengths = are_arrays_equal < 3u, 4u, std::array{ 1,2,3 }, std::array{ 1,2,3,4 } > {}();
		static constexpr auto not_equal_same_length = are_arrays_equal < 3u, 3u, std::array{ 4, 5, 6 }, std::array{ 1,2,3 }> {}();

		static constexpr auto arrays_equal = are_arrays_equal < 3u, 3u, std::array{ 1,2,3 }, std::array{ 3, 2,1 } > {};

		static_assert(equal && !(not_equal_same_length || not_equal_different_lengths));

		namespace detail
		{
			template<auto PREDICATE_OBJECT_INST>
			struct predicate_object_instance_wrapper
			{
				static constexpr auto predicate_object = PREDICATE_OBJECT_INST;
				static constexpr auto predicate_result = predicate_object();
				using predicate_result_type = std::remove_cvref_t<decltype(predicate_result)>;

			};
		}

		template<auto PREDICATE_OBJECT_INST>
		using predicate_result_type_t
			= typename detail::predicate_object_instance_wrapper<PREDICATE_OBJECT_INST>::predicate_result_type;



		static constexpr auto four_plus_nine = consteval_adder<4, 9>{};

		static_assert(is_valid_literal_value<four_plus_nine>);
		static_assert(four_plus_nine() == 13);
	}

	template<auto PREDICATE_OBJECT_INST>
	concept predicate_func_obj_instance = std::is_trivially_copyable_v<detail::predicate_result_type_t<PREDICATE_OBJECT_INST>> && requires
	{
		{detail::detail::predicate_object_instance_wrapper<PREDICATE_OBJECT_INST>::predicate_result} -> nothrow_convertible_to<bool>;
	};

	static_assert(predicate_func_obj_instance<detail::four_plus_nine>);

	static_assert(predicate_func_obj_instance<detail::arrays_equal>);

	inline consteval void consteval_assert(bool condition, std::string_view on_fail) noexcept
	{
		if (!condition)
		{
			consteval_fail(on_fail);
		}
	}

#ifdef  __cpp_lib_to_underlying
	static constexpr bool has_to_underlying = true;

	template<is_enum TEnum>
	constexpr auto to_underlying_proxy<TEnum>::operator()(TEnum val) const noexcept -> under_t
	{
		return std::to_underlying(val);
	}

#else
	static constexpr bool has_to_underlying = false;

	template<is_enum TEnum>
	constexpr auto to_underlying_proxy<TEnum>::operator()(TEnum val) const noexcept -> under_t
	{
		return std::bit_cast<under_t>(val);
	}
#endif

	struct size_t_lit_proxy  // NOLINT(cppcoreguidelines-special-member-functions)
	{
		// ReSharper disable CppRedundantCastExpression
		static constexpr unsigned long long max_size_t = static_cast<unsigned long long>(std::numeric_limits<std::size_t>::max());
		// ReSharper restore CppRedundantCastExpression
		[[nodiscard]] consteval std::size_t operator()(unsigned long long value) const noexcept;
		consteval size_t_lit_proxy() noexcept = default;
		constexpr ~size_t_lit_proxy() = default;
	};


	consteval std::size_t size_t_lit_proxy::operator()(unsigned long long value) const noexcept
	{
		using namespace std::string_view_literals;
		constexpr unsigned long long max_size_t_value = std::numeric_limits<std::size_t>::max();
		consteval_assert(value <= max_size_t_value, // NOLINT(clang-diagnostic-tautological-type-limit-compare)
			"value supplied exceeds the range of std::size_t."sv);

		// ReSharper disable once CppRedundantCastExpression -- not redundant if std::size_t is 32 (or 16?) bit.
		return static_cast<std::size_t>(value);
	}

}

namespace cps::std_ext
{
	using internal::to_underlying_proxy;

	template<internal::is_enum TEnum>
	static constexpr auto conversion_proxy_v = to_underlying_proxy<TEnum>{};


	 template<internal::is_enum TEnum>
	constexpr auto to_underlying(TEnum enumVal) noexcept
		-> std::underlying_type_t<TEnum>
	{
		constexpr auto proxy = conversion_proxy_v<TEnum>;
		return proxy(enumVal);
	}

    consteval void consteval_assert(bool condition, std::string_view msg_on_fl) noexcept;

	namespace literals
	{
		constexpr auto lit_proxy = internal::size_t_lit_proxy{};

	 	consteval std::size_t operator""_szt(unsigned long long v) noexcept
		{
			return lit_proxy(v);
		}
	}



	[[noreturn]] static inline void unreachable()
	{
		internal::unreachable_impl();
	}

    consteval void consteval_assert(bool condition, std::string_view msg_on_fl) noexcept
    {
        if (!condition)
        {
            std::cerr << msg_on_fl << std::endl;
            unreachable();
        }
    }
}



#endif // CPS_CTIME_SV_STD_EXT_HPP
