#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Arena.hpp"
#include "CommonTypes.hpp"
#include "Dispatcher.hpp"
#include "ErrorCode.inl"
#include "Logger.hpp"
#include "Phases.hpp"
#include "SimulationData.hpp"
#include "SimulationEvents.hpp"
#include "SimulationState.hpp"
#include "System.hpp"
#include "Time.hpp"

namespace Enoki
{
	struct StepInput
	{
		EventQueue inputEvents;
	};

	struct SimulationPhases
	{
		// phases for the initialisation process
		std::vector<Phase> init;

		// phases and their order
		std::vector<Phase> step;
	};

	class Simulation
	{
	public:
		Simulation(size_t systemArenaCapacity)
			: systems({.arena = Arena(systemArenaCapacity)})
		{}

		// allocates and sets up system within the simulation.
		template <std::derived_from<System> T, typename... Args>
		bool RegisterSystem(Args... args)
		{
			OffsetPtr<T> systemPtr = systems.arena.New<T>(std::forward<decltype(args)>(args)...);

			T* system = systems.arena.AccessPtr<T>(systemPtr);

			systems.lookup.emplace(system->GetName().data(), system);

			ENOKI_INFO(Simulation, "Registered system {}", system->GetName().data());
			return true;
		}

		void SetPhases(const SimulationPhases& inPhases)
		{
			phases = inPhases;
		}

		// raw pointers here. this is because we never re-bind systems.
		System* GetSystem(std::string_view name)
		{
			return systems.lookup.at(std::string(name));
		}

		void RegisterSystemListeners()
		{
			for (auto& [name, system] : systems.lookup)
			{
				system->RegisterListeners(dispatchers);
			}
		}

		void BindSystems()
		{
			assert(currentState);

			for (auto& [name, system] : systems.lookup)
			{
				system->BindSimulationData(&currentState->data);
			}
		}

		void BindSimulationState(std::unique_ptr<SimulationState> state)
		{
			currentState = std::move(state);
		}

		void Initialise()
		{
			BindSystems();

			// add step info to the simulation state
			currentState->data.NewNamed<StepInfo>("StepInfo",
				StepInfo {
					.deltaTimeMS = MillisecondsU64(1000),
					.stepTimeMS = MillisecondsU64(0),
					.stepNumber = 0,
				});

			RegisterSystemListeners();

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

			StepInfo& currentStepInfo = currentState->data.AccessNamed<StepInfo>("StepInfo");
			currentStepInfo.stepNumber++;
			currentStepInfo.stepTimeMS += currentStepInfo.deltaTimeMS;

			for (const Phase& phase : phases.step)
			{
				// We dispatch normal events BEFORE steppers
				dispatchers.DispatchEvents(phase, events);
				dispatchers.DispatchStepper(phase, currentStepInfo);

				// flush pending inputs for future phases to use
				events.FlushPending();
			}
		}

	private:
		struct
		{
			std::unordered_map<std::string, System*> lookup;
			Arena arena;
		} systems;

		SimulationPhases phases;

		// one per phase
		PhasedDispatchers dispatchers;

		// the data and events we are outputting to. this can be allocated
		// anywhere!
		std::unique_ptr<SimulationState> currentState = nullptr;
	};
} // namespace Enoki

namespace Enoki
{
	class Engine
	{
	public:
		Engine(ISimulationStatePool* pool, size_t systemArenaCapacity)
			: simulation(systemArenaCapacity)
			, statePool(pool)
		{}

		Simulation& GetSimulation()
		{
			return simulation;
		}

		ErrorCode Initialise()
		{
			// register systems here
			simulation.BindSimulationState(statePool->Get());
			simulation.Initialise();

			return Success;
		}

		void Update()
		{
			Arena stepArena(2_MiB);

			StepInput stepInput {
				.inputEvents = EventQueue(&stepArena),
			};

			while (true)
			{
				simulation.Step(stepInput);
			}
		}

	private:
		Simulation simulation;
		ISimulationStatePool* statePool;
	};
} // namespace Enoki