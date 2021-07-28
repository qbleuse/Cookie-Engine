#ifndef _CGP_WORKER_HPP__
#define _CGP_WORKER_HPP__

#include "ComponentTransform.hpp"
#include "Map.hpp"
#include "Gameplay/CGPResource.hpp"
#include "Gameplay/Income.hpp"
#include <vector>

namespace Cookie
{
	namespace ECS
	{
		class Coordinator;
	}

	namespace Resources
	{
		class Prefab;
		class Map;
	}

	namespace Gameplay
	{
		#define TIME_TO_HARVEST 2
		#define PRIMARY_PER_HARVEST 5
		#define SECONDARY_PER_HARVEST 4

		class CGPWorker
		{
		public:

			//will be replace by the CPGMove with flying later on
			float moveSpeed                  {5};

			//Harvest
			Income* income                   {nullptr};
			Core::Math::Vec3* posBase        {nullptr};
			Core::Math::Vec3* posResource    {nullptr};
			CGPResource*      resource       {nullptr};
			float harvestCountdown           {0};
			bool isCarryingResource          {false};

			//Building
			std::vector<Resources::Prefab*>	possibleBuildings;
			std::vector<std::string>		possibleBuildingsAtLoad;
			Resources::Prefab*				BuildingInConstruction {nullptr};
			Core::Math::Vec3				posBuilding            {0,  0, 0}; // = mousePos when start construction
			bool							needTostartBuilding    {false};
			float                           constructionCountdown  {0};
			std::vector<Resources::Tile*>   occupiedTiles; //get at start of building, then set in building

			CGPWorker() {}
			~CGPWorker() {}

			inline void ToDefault() noexcept
			{
				income						= nullptr;
				posBase                     = nullptr;
				posResource                 = nullptr;
				resource					= nullptr;
				harvestCountdown            = 0;
				isCarryingResource          = false;

				possibleBuildings.clear();
				possibleBuildingsAtLoad.clear();
				BuildingInConstruction      = nullptr;
				posBuilding                 = {0, 0, 0};
				needTostartBuilding         = false;
				constructionCountdown       = 0;
				for (int i = 0; i < occupiedTiles.size(); ++i)
					occupiedTiles[i]->isObstacle = false;
				occupiedTiles.clear();
			}

			void Update(Resources::Map& map, ECS::Coordinator& coordinator, int selfId);

			void SetResource(Core::Math::Vec3& resourcePos, CGPResource& resourceCGP);
			bool StartBuilding(Resources::Map& map, Core::Math::Vec3& _posBuilding, int indexInPossibleBuildings);
		};

	}
}

#endif // !_CGP_WORKER_HPP__
