#pragma once

/* referenced this
 * https://bastian.rieck.me/blog/2015/event_system_cxx11/
 */

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <map>
#include <string_view>
#include <vector>

#include "CommonTypes.hpp"
#include "Phases.hpp"
#include "SimulationEvents.hpp"

namespace Enoki
{
	class Dispatcher
	{
	public:
		using RawCallback = std::function<void(const uint8*)>;

		template <typename T>
		using Callback = std::function<void(const T&)>;

		struct CallbackBucket
		{
			size_t eventSize;
			std::vector<RawCallback> callbacks;
		};

		template <typename Event, typename System>
		void RegisterListener(std::string_view name, Callback<Event>& callback,
		                      System* system)
		{
			CallbackBucket& bucket = callbacks[name.data()];

			// initialise the size of the bucket if it's the first event in that
			// bucket
			if (bucket.callbacks.size() == 0)
			{
				bucket.eventSize = sizeof(Event);
			}
			else
			{
				assert(sizeof(Event) == bucket.eventSize);
			}

			// The function wrapper will cast the raw pointer representation to
			// the expected T& for the callback. The functor carries a functor
			// to the callback, and a system pointer to call it on.
			bucket.callbacks.emplace_back(
				[=](const uint8* rawData) -> void
				{ system->callback(reinterpret_cast<const Event&>(rawData)); });
		};

		// Dispatches a single event to all listeners of that event
		void DispatchRawEvent(std::string_view name, const uint8* data,
		                      size_t size) const
		{
			assert(callbacks.contains(name.data()));
			const CallbackBucket& bucket = callbacks.at(name.data());

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
				DispatchRawEvent(header.name, simEvents.AccessEventData(header),
				                 header.size);
			}
		}

	private:
		std::map<std::string, CallbackBucket> callbacks;
	};

	class PhaseDispatchers
	{
	public:
		template <typename Event, typename System>
		void RegisterListener(const Phase& phase, std::string_view name,
		                      Dispatcher::Callback<Event>& callback,
		                      System* system)
		{
			dispatchers[phase].RegisterListener(name, callback, system);
			std::printf("Registered listener {%s}:{%s} at phase {%s}",
			            system->GetName(), name, phase.name);
		}

		void DispatchEvents(const Phase& phase, SimulationEvents& events) const
		{
			dispatchers.at(phase).DispatchEvents(events);
		}

	private:
		std::map<Phase, Dispatcher> dispatchers;
	};
} // namespace Enoki