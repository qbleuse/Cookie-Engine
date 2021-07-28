#ifndef _CGP_PRODUCER_HPP__
#define _CGP_PRODUCER_HPP__

#include "Map.hpp"
#include "Gameplay/Income.hpp"
#include <vector>

namespace Cookie
{
	namespace Resources
	{
		class Prefab;
	}
	namespace ECS
	{
		class Coordinator;
	}

	namespace Gameplay
	{

		#define FLYING_Y_LEVEL 6
		#define CGP_PRODUCER_MAX_IN_QUEUE 5

		class CGPProducer
		{
		public:
			ECS::ComponentTransform* 			trs {nullptr};
			Income*							income {nullptr};
			int								supplyGiven {0};

			std::vector<Resources::Prefab*>	possibleUnits;
			std::vector<std::string>		possibleUnitsAtLoad; //for serialization

			std::vector<Resources::Prefab*> queueOfUnits;
			float                           currentCountdown {0};
			Core::Math::Vec3				newUnitDestination {0, 0, 0};

			Core::Math::Vec2                tileSize {{0, 0}};
			std::vector<Resources::Tile*>   occupiedTiles;

			CGPProducer() {}
			~CGPProducer() {}


			inline void ToDefault() noexcept
			{
				income = nullptr;
				supplyGiven = 0;

				possibleUnits.clear();
				possibleUnitsAtLoad.clear();

				queueOfUnits.clear();
				currentCountdown = 0;
				newUnitDestination = {0, 0, 0};
				
				tileSize = { {0, 0} };
				for (int i = 0; i < occupiedTiles.size(); ++i)
					occupiedTiles[i]->isObstacle = false;
				occupiedTiles.clear();
				
			}
		
			void UpdateCountdown(Resources::Map& map, ECS::Coordinator& coordinator, int selfId);

			bool AddUnitToQueue(int indexInPossible);
			void RemoveUnitFromQueue(int indexInQueue);

		};
	}
}

#endif // !_CGP_PRODUCER_HPP__