#pragma once
#include "Arena.hpp"
#include "Event.hpp"
#include <cstddef>
#include <cstring>
#include <vector>

namespace Enoki
{
	struct EventQueue
	{
		EventQueue(Arena* eventArena)
			: arena(eventArena)
		{}

		// we allocate our events into this arena
		Arena* arena;
		std::vector<EventHeader> events;

		void QueueBytes(EventID id, const uint8* data, size_t size)
		{
			OffsetPtr<uint8> ptr = arena->AllocBytes(size);
			uint8* bytesPtr = arena->AccessPtr(ptr);

			std::memcpy(bytesPtr, data, size);

			events.push_back(EventHeader {.id = id, .ptr = ptr, .size = size});
		}

		template <typename Event, typename... Args>
		void QueueEvent(EventID id, Args... args)
		{
			OffsetPtr<Event> ptr = arena->New<Event>(args...);
			events.push_back(EventHeader {
				.id = id,
				.ptr = ptr,
				.size = sizeof(Event),
			});
		}

		template <typename Event, typename... Args>
		void QueueEvent(Args... args)
		{
			EventID id {0}; // generate this from the typehash or registration

			OffsetPtr<Event> ptr = arena->New<Event>(args...);
			events.push_back(EventHeader {
				.id = id,
				.ptr = ptr,
				.size = sizeof(Event),
			});
		}

		void AppendQueue(const EventQueue& other)
		{
			for (auto& event : other.events)
			{
				const uint8* eventData = other.AccessEventData(event);
				QueueBytes(event.id, eventData, event.size);
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