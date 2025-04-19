#pragma once
#include <cstddef>
#include <string>

namespace Enoki
{
	struct Phase
	{
		std::string name;
		// more phase data can be added here

		bool operator==(const Phase& other) const
		{
			return name == other.name;
		}
	};

} // namespace Enoki


template <>
struct std::hash<Enoki::Phase>
{
	std::size_t operator()(const Enoki::Phase& phase) const
	{
		return std::hash<std::string>{}(phase.name);
	}
};