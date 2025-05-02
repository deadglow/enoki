#pragma once

#include "Dispatcher.hpp"
#include "Logger.hpp"
#include "System.hpp"
#include "TestPhases.hpp"
#include <cstdio>

namespace Events
{
	struct TestEvent
	{
		int data;
	};
}; // namespace Events

class TestSystem : public Enoki::System
{
public:
	TestSystem()
		: Enoki::System("TestSystem")
	{}

	void RegisterListeners(Enoki::PhasedDispatchers& dispatchers) override
	{
		dispatchers.RegisterListener<Events::TestEvent>(StepPhases::Step, &TestSystem::OnTestEvent, this);

		dispatchers.RegisterStepper(StepPhases::Step, &TestSystem::Step, this);
	}

	void OnTestEvent(const Events::TestEvent& event)
	{
		ENOKI_INFO(TestSystem, "Test event data: {}", event.data);
	}

	void Step(const Enoki::StepInfo& stepInfo)
	{
		ENOKI_INFO(TestSystem, "Step! {}", stepInfo.stepNumber);
	}

private:
};