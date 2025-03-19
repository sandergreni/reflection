
#ifndef __REFLECTION_H__
#define __REFLECTION_H__

#include <boost/pfr.hpp>

namespace vs::reflection
{
	template <typename T>
	concept reflective = std::is_aggregate<T>::value;

	template <typename T, typename Func, std::size_t... I>
	void member_invoker(T& type, Func&& func, std::index_sequence<I...>)
	{
		((func(boost::pfr::get_name<I, T>(), boost::pfr::get<I>(type))), ...);
	}

	template <typename T, typename Func>
	void for_each_member(T& type, Func&& func)
	{
		member_invoker(type, std::forward<Func>(func), std::make_index_sequence<boost::pfr::tuple_size_v<T>>());
	}
}

#endif