#pragma once

/* referenced this
 * https://bastian.rieck.me/blog/2015/event_system_cxx11/
 */

#include <cassert>
#include <functional>
#include <limits>
#include <unordered_map>
#include <vector>

#include "CommonTypes.hpp"
#include "Event.hpp"
#include "Logger.hpp"
#include "Phases.hpp"
#include "SimulationEvents.hpp"
#include "Time.hpp"

namespace Enoki
{
	class Dispatcher
	{
	public:
		using RawCallback = std::function<void(const uint8*)>;

		struct CallbackBucket
		{
			std::string eventName;
			size_t eventSize;
			std::vector<RawCallback> callbacks;
		};

		template <typename Event, typename BoundSystem>
		void RegisterListener(EventID eventID, auto callback, BoundSystem* system)
		{
			CallbackBucket& bucket = callbacks[eventID];

			// initialise the size of the bucket if it's the first event in that
			// bucket
			if (bucket.callbacks.size() == 0)
			{
				bucket.eventSize = sizeof(Event);
				bucket.eventName = GetTypeName<Event>();
			}
			else
			{
				assert(sizeof(Event) == bucket.eventSize);
			}

			// The function wrapper will cast the raw pointer representation
			// to the expected T& for the callback. The functor carries a
			// functor to the callback, and a system pointer to call it on.
			bucket.callbacks.emplace_back(
				[=](const uint8* rawData) -> void { (system->*callback)(reinterpret_cast<const Event&>(rawData)); });
		};

		// Dispatches a single event to all listeners of that event
		void DispatchRawEvent(EventID eventID, const uint8* data, size_t size) const
		{
			assert(callbacks.contains(eventID));
			const CallbackBucket& bucket = callbacks.at(eventID);

			// Don't wanna pass an invalid struct in here!
			assert(size == bucket.eventSize);

			for (auto& callback : bucket.callbacks)
			{
				callback(data);
			}
		}

		void DispatchEvents(const SimulationEvents& simEvents) const
		{
			auto& eventHeaders = simEvents.events;
			for (int i = 0; i < simEvents.firstPendingEvent; ++i)
			{
				const EventHeader& header = eventHeaders[i];
				DispatchRawEvent(header.id, simEvents.AccessEventData(header), header.size);
			}
		}

	private:
		std::unordered_map<EventID, CallbackBucket> callbacks;
	};

	static const EventID StepEventID {0};
	static const EventID InvalidEventID {std::numeric_limits<size_t>::max()};

	class PhasedDispatchers
	{
	public:
		template <typename Event>
		void RegisterListener(const Phase& phase, EventID eventID, auto callback, auto* system)
		{
			dispatchers[phase].RegisterListener<Event>(eventID, callback, system);

			ENOKI_INFO(
				PhasedDispatchers, "Registered listener {}:{} at phase {}", system->GetName().data(), eventID.id, phase.name.c_str());
		}

		template <typename Event>
		void RegisterListener(const Phase& phase, auto callback, auto* system)
		{
			RegisterListener<Event>(phase, GetEventID<Event>(), callback, system);
		}

		void RegisterStepper(const Phase& phase, auto callback, auto* system)
		{
			RegisterListener<StepInfo>(phase, callback, system);
		}

		void DispatchEvents(const Phase& phase, SimulationEvents& events) const
		{
			if (dispatchers.contains(phase))
			{
				dispatchers.at(phase).DispatchEvents(events);
			}
		}

		void DispatchStepper(const Phase& phase, const StepInfo& stepInfo) const
		{
			if (dispatchers.contains(phase))
			{
				dispatchers.at(phase).DispatchRawEvent(GetEventID<StepInfo>(), reinterpret_cast<const uint8*>(&stepInfo), sizeof(StepInfo));
			}
		}

	private:
		std::unordered_map<Phase, Dispatcher> dispatchers;
	};
} // namespace Enoki