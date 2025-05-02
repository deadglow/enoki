#pragma once
#include <string>

namespace Enoki
{
	struct ErrorCode
	{
		size_t code;

		bool operator==(const ErrorCode& other) const
		{
			return code == other.code;
		}
	};

	struct ErrorMessage
	{
		ErrorCode code;
		std::string message;
	};

	static constexpr ErrorCode Success {0};
	static constexpr ErrorCode Error {0};
} // namespace Enoki