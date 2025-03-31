#pragma once

#include "CommonTypes.hpp"
#include "Arena.hpp"
#include <string>
#include <vector>
#include <unordered_map>

constexpr size_t ArenaCapacity = (2 * 1024 * 1024);

struct SimulationData
{
	struct Allocation
	{
		size_t address;
		size_t size;
		std::string id;
	};

	template <typename T>
	struct Ptr
	{
		T* ptr;
	};

	Arena<ArenaCapacity> data;
	std::vector<Allocation> allocations;
	std::unordered_map<uint64, size_t> namedAllocations;

	template <typename T>
	T* Alloc()
	{
		T* result = data.Alloc<T>();
		registry.push_back({
			.address = static_cast<size_t>(result),
			.size = sizeof(T),
			.id = std::string()
		});
		return result;
	}

	template <typename T>
	T* Alloc(const char* name)
	{
		T* result = data.Alloc<T>();
		registry.push_back({
			.address = static_cast<size_t>(result),
			.size = sizeof(T),
			.id = std::string()
		});
		return result;
	}
}

struct SimulationState
{
	std::vector<Events> events;
};

class Simulation
{
public:

private:
	
};