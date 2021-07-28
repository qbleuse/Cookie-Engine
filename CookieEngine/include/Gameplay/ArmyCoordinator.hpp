#ifndef _ARMY_COORDINATOR_HPP__
#define _ARMY_COORDINATOR_HPP__

#include <vector>
#include "Gameplay/AIBehavior.hpp"

namespace Cookie
{
	namespace Resources
	{
		class Map;
	}

	namespace Gameplay
	{
		class Army;

		enum E_GOALS
		{
			//Economic
			E_DEVELOP_WORKER,
			E_DEVELOP_BASE,
			E_DEVELOP_ARMY,
			E_WAIT_ECONOMIC,

			//Military
			E_DEFENSE,
			E_ATTACK,
			E_RETREAT,
			E_WAIT_MILITARY
		};

		#define NB_TILES_BETWEEN_BUILDINGS 2

		class ArmyCoordinator
		{
		public :

			Army*                army {nullptr};
			std::vector<E_GOALS> goals;

			AIBehavior*			 behavior{nullptr};

			int nbOfWorkerInProduction   {0};
			int nbOfBuildingInProduction {0};
			int nbOfUnitInProduction     {0};
			bool canAttack				 {true};
			bool canDefend               {true};

			int					currentStepIndex{0};
			AIStep				stepGoals; //total

			ArmyCoordinator(Army* _army, AIBehavior* _aiBehavior) : army{_army}, behavior{_aiBehavior} {}
			~ArmyCoordinator() {}


			//void Tactical(); done by the CGPAttack
			void Analysis();
			void ResourceAllocation(Resources::Map& map);

		private:
			void DevelopWorker();
			void DevelopBase(Resources::Map& map);
			void DevelopArmy();
			void Attack(Resources::Map& map);
			void Defense(Resources::Map& map);
			void Retreat();

			void AddNextStep()
			{
				if (currentStepIndex < behavior->steps.size())
				{
					stepGoals += behavior->steps[currentStepIndex];
					currentStepIndex++;
				}
				//else loop with last Step goals
			}
		};


	}
}

#endif // !_ARMY_COORDINATOR_HPP__