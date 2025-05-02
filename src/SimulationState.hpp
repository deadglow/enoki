#pragma once

#include "Arena.hpp"
#include "CommonTypes.hpp"
#include "SimulationData.hpp"
#include "SimulationEvents.hpp"
#include <cassert>
#include <cstddef>
#include <memory>
#include <queue>

namespace Enoki
{
	class SimulationState
	{
	public:
		SimulationState(size_t dataCapacity, size_t eventCapacity)
			: dataArena(dataCapacity)
			, eventsArena(eventCapacity)
			, data(&dataArena)
			, events(&eventsArena)
		{}

		Arena dataArena;
		Arena eventsArena;

		SimulationData data;
		SimulationEvents events;
	};
} // namespace Enoki

namespace Enoki
{
	class ISimulationStatePool
	{
	public:
		virtual bool Available() const = 0;
		virtual std::unique_ptr<SimulationState> Get() = 0;
		virtual void Free(std::unique_ptr<SimulationState>& state) = 0;
	};

	template <size_t DataCapacity, size_t EventCapacity, size_t InstanceCount>
	class SimulationStatePool : public ISimulationStatePool
	{
	public:
		SimulationStatePool()
		{
			for (int i = 0; i < InstanceCount; ++i)
			{
				freelist.push(std::make_unique<SimulationState>(DataCapacity,
				                                                EventCapacity));
			}
		}

		bool Available() const override { return !freelist.empty(); }

		std::unique_ptr<SimulationState> Get() override
		{
			assert(Available());

			std::unique_ptr<SimulationState> freeState =
				std::move(freelist.front());
			freelist.pop();
			return freeState;
		};

		void Free(std::unique_ptr<SimulationState>& state) override
		{
			freelist.push(std::move(state));
		}

	private:
		std::queue<std::unique_ptr<SimulationState>> freelist;
		std::queue<std::unique_ptr<SimulationState>> consumeList;
	};
} // namespace Enoki