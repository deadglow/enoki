#pragma once

#include "CommonTypes.hpp"

template <size_t _Capacity>
class Arena
{
public:
	static constexpr size_t Capacity = _Capacity;

public:

	size_t GetUsed() const
	{
		return used;
	}

	size_t GetAvailable() const
	{
		return Capacity - used;
	}

	void* AllocRawAligned(size_t size, size_t alignment)
	{
		// todo
	}

	void* AllocRaw(size_t size)
	{
		assert(size)
		size_t ptr = memory + used;
		used += size;
		return ptr;
	}

	void Free(size_t size)
	{
		assert(size <= used);
		assert(size > 0);

		used -= size;
	}

	template<typename T>
	T* Alloc()
	{
		return static_cast<T*>(AllocRaw(sizeof(T)));
	}

	template<typename T>
	T* New(auto&&... args)
	{
		T* ptr = Alloc<T>();
		// @todo: wtf is going on below? placement new, then forward args, but why decltype?
		new(ptr) T(std::forward<decltype(args)>(args)...);
		return ptr;
	}

private:
	uint8 memory[Capacity];
	size_t used;
};