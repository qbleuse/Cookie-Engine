#include "Core/Time.hpp"
#include "Physics/PhysicsSimulator.hpp"

using namespace Cookie::Physics;

constexpr float initTimeStep = 1.0f / 60.0f;

/*================ CONSTRUCTORS/DESTRUCTORS ================*/

PhysicsSimulator::PhysicsSimulator():
	timeStep{initTimeStep}
{
}

PhysicsSimulator::~PhysicsSimulator()
{
}

/*================ REALTIME METHODS ================*/

void PhysicsSimulator::Update()
{
	accumulator += Cookie::Core::DeltaTime();

	while (accumulator >= timeStep)
	{
		worldSim->update(timeStep);

		accumulator -= timeStep;
	}

	/* factor is usually over 1 as timeStep is between 0 and 1 usually.
	 * but it is usually not over 1 when update is really done 
	 * as accumulator will be lower than timeStep */
	factor = accumulator / timeStep;
}