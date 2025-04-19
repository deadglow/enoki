#pragma once
#include "Arena.hpp"
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

namespace Enoki
{
	struct EventHeader
	{
		std::string name; // make this a fixed string
		OffsetPtr<uint8> ptr;
		size_t size;
	};

	struct EventQueue
	{
		EventQueue(Arena* eventArena)
			: arena(eventArena)
		{}

		// we allocate our events into this arena
		Arena* arena;
		std::vector<EventHeader> events;

		void QueueBytes(std::string_view name, const uint8* data, size_t size)
		{
			OffsetPtr<uint8> ptr = arena->AllocBytes(size);
			uint8* bytesPtr = arena->AccessPtr(ptr);

			std::memcpy(bytesPtr, data, size);

			events.push_back(EventHeader {
				.name = name.data(),
				.ptr = ptr,
				.size = size
			});
		}

		template <typename Event, typename... Args>
		void QueueEvent(std::string_view name, Args... args)
		{
			OffsetPtr<Event> ptr = arena->New<Event>(args...);
			events.push_back(EventHeader {
				.name = name.data(),
				.ptr = ptr,
				.size = sizeof(Event),
			});
		}
		
		void AppendQueue(const EventQueue& other)
		{
			for (auto& event : other.events)
			{
				const uint8* eventData = other.AccessEventData(event);
				QueueBytes(event.name, eventData, event.size);
			}
		}

		const uint8* AccessEventData(const EventHeader& event) const
		{
			return arena->AccessPtr(event.ptr);
		}

		void ClearQueue()
		{
			events.clear();
			arena->Clear();
		}

		size_t Size() { return events.size(); }
	};

	struct SimulationEvents : public EventQueue
	{
		SimulationEvents(Arena* eventArena)
			: EventQueue(eventArena)
		{}
		
		// tracker so that we don't dispatch events that are being added to the
		// queue WHILE iterating.
		size_t firstPendingEvent = 0;

		void FlushPending() { firstPendingEvent = events.size(); }

		void Reset()
		{
			ClearQueue();
			FlushPending();
		}
	};
} // namespace Enoki