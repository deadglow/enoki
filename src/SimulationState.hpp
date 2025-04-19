#pragma once

#include "Arena.hpp"
#include "CommonTypes.hpp"
#include "SimulationData.hpp"
#include "SimulationEvents.hpp"
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <memory>
#include <queue>

namespace Enoki
{
	class SimulationState
	{
	public:
		SimulationState(Arena* dataArena, Arena* eventsArena)
			: data(dataArena)
			, events(eventsArena)
		{}

		SimulationData data;
		SimulationEvents events;
	};

	template <size_t DataCapacity, size_t EventCapacity>
	class SimulationStateInplace : public SimulationState
	{
	public:
		SimulationStateInplace()
			: SimulationState(&dataArena, &eventsArena)
		{}

		SimulationStateInplace(const SimulationStateInplace& other)
		{
			dataArena = other.dataArena;
			eventsArena = other.eventsArena;

			data = other.data;
			events = other.events;

			data.arena = &dataArena;
			events.arena = &eventsArena;
		};

		SimulationStateInplace& operator=(const SimulationStateInplace& other)
		{
			this(other);
			return this;
		};

		ArenaInplace<DataCapacity> dataArena;
		ArenaInplace<EventCapacity> eventsArena;
	};

} // namespace Enoki

namespace Enoki
{
	class ISimulationStatePool
	{
		public:
		virtual bool Available() const = 0;
		virtual std::unique_ptr<SimulationState> Get() = 0;
		virtual void Free(const std::unique_ptr<SimulationState>& state) = 0;
	};

	template <size_t DataCapacity, size_t EventCapacity, size_t PoolCapacity>
	class SimulationStatePool : public ISimulationStatePool
	{
	public:
		using SimState = SimulationStateInplace<DataCapacity, EventCapacity>;
	
		SimulationStatePool()
		{
			for (int i = 0; i < PoolCapacity; ++i)
			{
				freelist.push(std::make_unique<SimState>());
			}
		}

		bool Available() const override
		{
			return !freelist.empty();
		}

		std::unique_ptr<SimulationState> Get() override
		{
			assert(Available());

			std::unique_ptr<SimulationState> freeState = std::move(freelist.front());
			freelist.pop();
			return freeState;
		};

		void Free(const std::unique_ptr<SimulationState>& state) override
		{
			freelist.push(std::move(state));
		}

	private:
		std::queue<std::unique_ptr<SimulationState>> freelist;
		std::queue<std::unique_ptr<SimulationState>> consumeList;
	};
} // namespace Enoki