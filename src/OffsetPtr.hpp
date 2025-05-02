#pragma once
#include "CommonTypes.hpp"

namespace Enoki
{
	template <typename T>
	struct OffsetPtr
	{
		size_t offset;
	};
} // namespace Enoki