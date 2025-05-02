#pragma once
#include "CommonTypes.hpp"
#include "OffsetPtr.hpp"

namespace Enoki
{
	struct EventID
	{
		size_t id;
		bool operator==(const EventID& other) const = default;
	};

	template <typename T>
	consteval EventID GetEventID()
	{
		return EventID {HashType<T>()};
	}

	struct EventHeader
	{
		EventID id;
		OffsetPtr<uint8> ptr;
		size_t size;
	};
} // namespace Enoki

template <>
struct std::hash<Enoki::EventID>
{
	std::size_t operator()(const Enoki::EventID& id) const
	{
		return std::hash<size_t> {}(id.id);
	}
};

template <>
struct std::formatter<Enoki::EventID>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const Enoki::EventID& id, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", id.id);
	}
};