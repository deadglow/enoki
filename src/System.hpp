#pragma once
#include "Dispatcher.hpp"

namespace Enoki
{
	struct SimulationData;

	class System
	{
	public:
		System(const char* name)
			: name(name)
		{}

		std::string_view GetName() { return name; }

		void BindSimulationData(SimulationData* dataToBind)
		{
			data = dataToBind;
		}

		// Override this to add listeners
		virtual void RegisterListeners(PhaseDispatchers& dispatchers) {}

	private:
		std::string name;
		SimulationData* data = nullptr;
	};

} // namespace Enoki