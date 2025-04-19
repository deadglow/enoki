#pragma once

#include <concepts>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Arena.hpp"
#include "CommonTypes.hpp"
#include "Dispatcher.hpp"
#include "Phases.hpp"
#include "SimulationEvents.hpp"
#include "SimulationState.hpp"
#include "System.hpp"

namespace Enoki
{
	struct StepInput
	{
		EventQueue inputEvents;
	};

	template <size_t SystemArenaCapacity>
	class Simulation
	{
	public:
		// allocates and sets up system within the simulation.
		template <std::derived_from<System> T, typename... Args>
		bool RegisterSystem(Args... args)
		{
			OffsetPtr<T> systemPtr =
				systems.arena.New(std::forward<decltype(args)>(args)...);

			T* system = systems.arena.AccessPtr(systemPtr);

			systems.push_back(system);
			systems.lookup.insert(system->GetName(), system);

			std::printf("Registered system {%s}", system->GetName());
		}

		// raw pointers here. this is because we never re-bind systems.
		System* GetSystem(std::string_view name)
		{
			return systems.lookup.at(name);
		}

		void BindSystems()
		{
			assert(currentState);

			for (System* system : systems.list)
			{
				system->BindSimulationData(&currentState->data);
			}
		}

		void BindSimulationState(std::unique_ptr<SimulationState>& state)
		{
			currentState = std::move(state);
		}

		void Initialise()
		{
			BindSystems();

			for (const Phase& phase : phases.init)
			{
				// maybe we want to use a explicit loading event queue instead
				// of an output
				SimulationEvents& events = currentState->events;
				dispatchers.DispatchEvents(phase, events);

				// flush pending inputs for future phases to use
				events.FlushPending();
			}
		}

		void Step(const StepInput& input)
		{
			BindSystems();

			// Push in incoming events first!
			SimulationEvents& events = currentState->events;
			events.AppendQueue(input.inputEvents);

			for (const Phase& phase : phases.step)
			{
				dispatchers.DispatchEvents(phase, events);

				// flush pending inputs for future phases to use
				events.FlushPending();
			}
		}

	private:
		struct
		{
			std::vector<System*> list;
			std::unordered_map<std::string, System*> lookup;
			ArenaInplace<SystemArenaCapacity> arena;
		} systems;

		struct
		{
			// phases for the initialisation process
			std::vector<Phase> init;

			// phases and their order
			std::vector<Phase> step;
		} phases;

		// one per phase
		PhaseDispatchers dispatchers;

		// the data and events we are outputting to. this can be allocated
		// anywhere!
		std::unique_ptr<SimulationState> currentState = nullptr;
	};
} // namespace Enoki

namespace Enoki
{
	constexpr std::size_t SystemArenaCapacity = 2_MiB;
	class Engine
	{
	public:
		Engine(ISimulationStatePool* pool)
			: statePool(pool)
		{}

		void Initialise() {}

		void Update() {}

	private:
		Simulation<SystemArenaCapacity> simulation;
		ISimulationStatePool* statePool;
	};
} // namespace Enoki