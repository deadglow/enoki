#pragma once

#include "Logger.hpp"
#include <chrono>
#include <cstddef>

namespace Enoki::Literals
{
	constexpr size_t operator""_KiB(unsigned long long int x)
	{
		return 1024ULL * x;
	}

	constexpr size_t operator""_MiB(unsigned long long int x)
	{
		return 1024_KiB * x;
	}

	constexpr size_t operator""_GiB(unsigned long long int x)
	{
		return 1024_MiB * x;
	}

	constexpr size_t operator""_TiB(unsigned long long int x)
	{
		return 1024_GiB * x;
	}

	constexpr size_t operator""_PiB(unsigned long long int x)
	{
		return 1024_TiB * x;
	}
} // namespace Enoki::Literals

namespace Enoki
{
	using namespace Enoki::Literals;

	using size_t = std::size_t;

	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;

	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

	using MillisecondsU32 = std::chrono::duration<uint32, std::chrono::milliseconds::period>;
	using MillisecondsS32 = std::chrono::duration<int32, std::chrono::milliseconds::period>;
	using MillisecondsU64 = std::chrono::duration<uint64, std::chrono::milliseconds::period>;
	using MillisecondsS64 = std::chrono::duration<int64, std::chrono::milliseconds::period>;
	using MillisecondsF32 = std::chrono::duration<float, std::chrono::milliseconds::period>;
	using MillisecondsF64 = std::chrono::duration<double, std::chrono::milliseconds::period>;

	using SecondsU32 = std::chrono::duration<uint32, std::chrono::seconds::period>;
	using SecondsS32 = std::chrono::duration<int32, std::chrono::seconds::period>;
	using SecondsU64 = std::chrono::duration<uint64, std::chrono::seconds::period>;
	using SecondsS64 = std::chrono::duration<int64, std::chrono::seconds::period>;
	using SecondsF32 = std::chrono::duration<float, std::chrono::seconds::period>;
	using SecondsF64 = std::chrono::duration<double, std::chrono::seconds::period>;
} // namespace Enoki

namespace Enoki
{
	template <typename Str>
	consteval size_t HashString(const Str& toHash)
	{
		// For this example, I'm requiring size_t to be 64-bit, but you could
		// easily change the offset and prime used to the appropriate ones
		// based on sizeof(size_t).
		static_assert(sizeof(size_t) == 8);
		// FNV-1a 64 bit algorithm
		size_t result = 0xcbf29ce484222325; // FNV offset basis

		for (char c : toHash)
		{
			result ^= c;
			result *= 1099511628211; // FNV prime
		}

		return result;
	}

	template <typename T>
	consteval const char* GetTypeName()
	{
#ifdef _MSC_VER
		constexpr const char* name = __FUNCSIG__;
#else
		constexpr const char* name = __PRETTY_FUNCTION__;
#endif
		return name;
	}

	template <typename T>
	consteval size_t HashType()
	{
		return HashString(std::string_view(GetTypeName<T>()));
	}
} // namespace Enoki

#include "ErrorCode.inl"