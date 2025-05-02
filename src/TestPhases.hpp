#pragma once
#include "Phases.hpp"
#include <vector>

#define DEFINE_PHASE_TYPE(type)							\
namespace type##Phases::Internal						\
{														\
	static std::vector<Enoki::Phase> list {};			\
														\
	constexpr Enoki::Phase AddPhase(const char* name) {	\
		const Enoki::Phase phase {						\
			.name = name,								\
		};												\
		list.push_back(phase);							\
		return phase;									\
	}													\
}														\
namespace type##Phases									\

#define DEFINE_PHASE(name)									\
	inline Enoki::Phase name = Internal::AddPhase(#name);	\

DEFINE_PHASE_TYPE(Init)
{	
	DEFINE_PHASE(Begin)
	DEFINE_PHASE(First)
	DEFINE_PHASE(Second)
	DEFINE_PHASE(End)
}

DEFINE_PHASE_TYPE(Step)
{
	DEFINE_PHASE(Begin)
	DEFINE_PHASE(Step)
	DEFINE_PHASE(LateStep)
	DEFINE_PHASE(End)
}