#pragma once


template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename ... Args>
constexpr Scope<T> makeScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}


template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename ... Args>
constexpr Ref<T> makeRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}