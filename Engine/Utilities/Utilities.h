#pragma once

#define USE_STL_VECTOR 1
#define USE_STL_DEQUE  1

#if USE_STL_VECTOR
#include <vector>
namespace thomas::utilities
{
	template<typename T>
	using vector = std::vector<T>;
}

#endif // USE_STL_DEQUE

#if USE_STL_VECTOR
#include <deque>
namespace thomas::utilities
{
	template<typename T>
	using deque = std::deque<T>;
}

#endif // USE_STL_DEQUE

namespace thomas::utilities
{
	// TODO add method for vector and deque
}

