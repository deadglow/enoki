#pragma once
#include <cassert>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Arena.hpp"
#include "CommonTypes.hpp"

namespace Enoki
{
	template <typename T>
	struct DataPtr
	{
		OffsetPtr<T> ptr;
	};

	struct SimulationData
	{
		SimulationData(Arena* dataArena)
			: arena(dataArena)
		{}

		struct Allocation
		{
			std::string name;
			size_t offset;
			size_t size;
		};

		// must provide an arena for it to allocate in
		Arena* arena;

		// list of all the things it has allocated. lets allocate these in the
		// arena!
		std::vector<Allocation> allocations;
		std::unordered_map<std::string, Allocation> namedAllocations;

		template <typename T>
		DataPtr<T> Alloc()
		{
			OffsetPtr<T> result = arena->Alloc<T>();
			allocations.push_back({
				.name = "",
				.address = result.offset,
				.size = sizeof(T),
			});
			return {result};
		}

		template <typename T>
		DataPtr<T> Alloc(std::string_view name)
		{
			OffsetPtr<T> result = arena->Alloc<T>();
			allocations.push_back({
				.name = name,
				.address = result.offset,
				.size = sizeof(T),
			});

			assert(!namedAllocations.contains(name.data()));
			namedAllocations[name.data()] = allocations.back();

			return {result};
		}

		template <typename T>
		T& Access(DataPtr<T> ptr)
		{
			return *arena->AccessPtr<T>(ptr.ptr);
		}

		template <typename T>
		const T& Access(DataPtr<T> ptr) const
		{
			return *arena->AccessPtr<T>(ptr.ptr);
		}

		template <typename T>
		T& AccessNamed(std::string_view name)
		{
			assert(namedAllocations.contains(name.data()));
			const Allocation& allocation = namedAllocations.at(name.data());
			return Access(DataPtr<T>(allocation.offset));
		}

		template <typename T>
		const T& AccessNamed(std::string_view name) const
		{
			assert(namedAllocations.contains(name.data()));
			const Allocation& allocation = namedAllocations.at(name.data());
			return Access(DataPtr<T>(allocation.offset));
		}
	};

} // namespace Enoki