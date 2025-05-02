#pragma once

#include "CommonTypes.hpp"
#include "OffsetPtr.hpp"

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <utility>

namespace Enoki
{
	class Arena
	{
	public:
		Arena(size_t _capacity, std::allocator<uint8> allocator = std::allocator<uint8>())
			: capacity(_capacity)
		{
			buffer = std::unique_ptr<uint8[]>(allocator.allocate(capacity));
			std::memset(buffer.get(), 0, capacity);
		}

		inline size_t GetCapacity() const { return capacity; }

		inline size_t GetUsed() const { return used; }

		inline size_t GetAvailable() const { return capacity - used; }

		inline uint8* GetByteAddress(size_t offset) { return buffer.get() + offset; }

		template <typename T>
		inline T* AccessPtr(OffsetPtr<T> offset)
		{
			return reinterpret_cast<T*>(GetByteAddress(offset.offset));
		}

		inline OffsetPtr<uint8> AllocBytesAligned(size_t size, size_t alignment)
		{
			// todo
			return {0};
		}

		inline OffsetPtr<uint8> AllocBytes(size_t size)
		{
			assert(size);
			assert(size <= GetAvailable());

			OffsetPtr<uint8> ptr {used};
			used += size;
			return ptr;
		}

		void Clear() { used = 0; }

		template <typename T>
		inline OffsetPtr<T> Alloc()
		{
			return static_cast<OffsetPtr<T>>(AllocBytes(sizeof(T)).offset);
		}

		template <typename T, typename... Args>
		OffsetPtr<T> New(Args... args)
		{
			OffsetPtr<T> ptr = Alloc<T>();

			T* data = AccessPtr(ptr);
			new (data) T(std::forward<decltype(args)...>(args)...);
			return ptr;
		}

	protected:
		size_t capacity = 0;
		size_t used = 0;
		std::unique_ptr<uint8[]> buffer = nullptr;
	};
} // namespace Enoki
