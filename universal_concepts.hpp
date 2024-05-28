#ifndef CPS_CTIME_SV_CONCEPTS_UNIVERSAL_UNIVERSAL_CONCEPTS_HPP
#define CPS_CTIME_SV_CONCEPTS_UNIVERSAL_UNIVERSAL_CONCEPTS_HPP
#include <concepts>
namespace cps::concepts::universal
{
	template<typename TSrc, typename TDst>
	concept nothrow_convertible_to = std::convertible_to<TSrc, TDst>&& std::is_nothrow_convertible_v<TSrc, TDst>;

	template<typename TSrc, typename TDst>
	concept static_castable_to = std::convertible_to<TSrc, TDst> || requires (const TSrc & src)
	{
		{ static_cast<TDst>(src) } -> nothrow_convertible_to<TDst>;
	};

	template<typename TSrc, typename TDst>
	concept nothrow_static_castable_to = nothrow_convertible_to<TSrc, TDst> ||
		(static_castable_to<TSrc, TDst> && requires (const TSrc & src)
	{
		{ static_cast<TDst>(src) } noexcept -> nothrow_convertible_to<TDst>;
	});

	template<typename T>
	concept boolean_testable = std::convertible_to<T, bool> || static_castable_to<T, bool>;

	template<typename T>
	concept nothrow_boolean_testable = nothrow_convertible_to<T, bool> || nothrow_static_castable_to<T, bool>;
}


#endif // CPS_CTIME_SV_CONCEPTS_UNIVERSAL_UNIVERSAL_CONCEPTS_HPP