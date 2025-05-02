#include <cstddef>
#include <stdio.h>

#include "CommonTypes.hpp"
#include "Logger.hpp"
#include "Simulation.hpp"
#include "SimulationState.hpp"
#include "TestPhases.hpp"
#include "TestSystem.hpp"

using namespace Enoki::Literals;

int main(int argc, char* argv[])
{
	Enoki::SimulationStatePool<128_MiB, 128_MiB, 3> statePool;

	Enoki::Engine engine(&statePool, 128_MiB);

	Enoki::Simulation& simulation = engine.GetSimulation();
	simulation.SetPhases(Enoki::SimulationPhases {.init = InitPhases::Internal::list, .step = StepPhases::Internal::list});
	simulation.RegisterSystem<TestSystem>();

	Enoki::ErrorCode result = engine.Initialise();

	if (result == Enoki::Success)
	{
		ENOKI_INFO(Init, "Enoki initialised", NULL);
		// update ticker here pls
		engine.Update();
	}
	else
	{
		ENOKI_INFO(Init, "Enoki failed to init, error result: {}", result.code);
	}

	return result.code;
}