#ifndef __PHYSICS_SIMULATOR_HPP__
#define __PHYSICS_SIMULATOR_HPP__

#include <reactphysics3d/reactphysics3d.h>

namespace Cookie
{
	namespace Physics
	{
		/* the class that will be responsible 
		 * to update the physics in the game*/
		class PhysicsSimulator
		{
			/* accumulate the time between last update and next */
			private:
				float accumulator = 0.f;

			public:
				rp3d::PhysicsWorld* worldSim{ nullptr };

				/* time step is the frame rate usually (default is 1/60) */
				float timeStep		= .0f;
				/* factor between accumulated time and timeStep to sooth the displacement */
				float factor		= 0.f;


				PhysicsSimulator();
				~PhysicsSimulator();

				void Update();
		};
	}
}

#endif //__PHYSICS_SIMULATOR_HPP__