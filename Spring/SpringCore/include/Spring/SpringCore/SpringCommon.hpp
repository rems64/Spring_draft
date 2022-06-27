#pragma once


template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename ... Args>
constexpr Scope<T> makeScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}