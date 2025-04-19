#pragma once

#include "CommonTypes.hpp"
#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>

namespace Enoki
{
	template <typename T>
	struct OffsetPtr
	{
		size_t offset;
	};

	class Arena
	{
	public:
		static constexpr size_t Capacity = 322;

	public:
		inline size_t GetUsed() const { return used; }

		inline size_t GetAvailable() const { return Capacity - used; }

		inline size_t GetRawAddress(size_t offset)
		{
			// start of the arena DATA starts at the end of this structure.
			return (size_t)this + sizeof(Arena) + offset;
		}

		template <typename T>
		inline T* AccessPtr(OffsetPtr<T> offset)
		{
			return (T*)(GetRawAddress(offset.offset));
		}

		OffsetPtr<uint8> AllocBytesAligned(size_t size, size_t alignment)
		{
			// todo
		}

		OffsetPtr<uint8> AllocBytes(size_t size)
		{
			assert(size);
			OffsetPtr<uint8> ptr {used};
			used += size;
			return ptr;
		}

		void Free(size_t size)
		{
			assert(size <= used);
			assert(size > 0);

			used -= size;
		}

		void Clear() { used = 0; }

		template <typename T>
		inline OffsetPtr<T> Alloc()
		{
			return reinterpret_cast<OffsetPtr<T>>(AllocBytes(sizeof(T)));
		}

		template <typename T, typename... Args>
		OffsetPtr<T> New(Args... args)
		{
			OffsetPtr<T> ptr = Alloc<T>();

			T* data = ptr.Get(this);
			new (data) T(std::forward<decltype(args)>(args)...);
			return ptr;
		}

	protected:
		size_t used = 0;
	};

	template <size_t Capacity>
	class ArenaInplace final : public Arena
	{
	private:
		uint8 inplaceMemory[Capacity];
	};
} // namespace Enoki
