#pragma once
#include "CommonTypes.hpp"

namespace Enoki
{
	using HashResult = uint64;

	template <typename T>
	HashResult Hash(T& input)
	{
		return std::hash<T>{}(input);
	}
} // namespace Enoki