#ifndef _ARMY_HANDLER_HPP__
#define _ARMY_HANDLER_HPP__

#include "ECS/ComponentGameplay.hpp"
#include "Gameplay/ArmyCoordinator.hpp"
#include "Gameplay/Income.hpp"

namespace Cookie
{
	namespace Resources
	{
		class Map;
	}

	namespace Gameplay
	{

		#define MAX_ARMIES 8

		class Army
		{
		public:

			Gameplay::E_ARMY_NAME                name {Gameplay::E_ARMY_NAME::E_DEFAULT_NAME};
			Income                               income;
			std::vector<ECS::ComponentGameplay*> workers;
			std::vector<ECS::ComponentGameplay*> units;
			std::vector<ECS::ComponentGameplay*> buildings;

			Army() {}
			~Army() {}

			void Reset()
			{
				name = Gameplay::E_ARMY_NAME::E_DEFAULT_NAME;
				income.Reset();
				workers.clear();
				units.clear();
				buildings.clear();
			}

		};

		class ArmyHandler
		{
		public:
			int                          livingArmies {0};
			std::array<Army, MAX_ARMIES> armies;
			std::vector<ArmyCoordinator> armiesCoordinator;

			ArmyHandler() {}
			~ArmyHandler() {}

			void Debug();
			void UpdateArmyCoordinators(Resources::Map& map);

			Army*			 GetArmy(Gameplay::E_ARMY_NAME armyName);
			ArmyCoordinator* GetArmyCoordinator(Gameplay::E_ARMY_NAME armyName);

			void AddArmy(Gameplay::E_ARMY_NAME armyName);
			void RemoveArmy(Army& army);
			void AddArmyCoordinator(Gameplay::E_ARMY_NAME armyName, AIBehavior* aiBehavior);
			void RemoveArmyCoordinator(Gameplay::E_ARMY_NAME armyName);

			void AddElementToArmy(ECS::ComponentGameplay* element);
			void AddElementToArmy(Army& army, ECS::ComponentGameplay* element);
			void RemoveElementFromArmy(ECS::ComponentGameplay* element, std::string entityName);
			void RemoveElementFromArmy(Army& army, ECS::ComponentGameplay* element, std::string entityName);
			void RemoveElementFromVector(std::vector<ECS::ComponentGameplay*>& vector, ECS::ComponentGameplay* element);

			void RemoveArmyIfEmpty(Army& army);
			void PlayerWin();
			void PlayerLose();
		};

	}
}

#endif // !_ARMY_HANDLER_HPP__