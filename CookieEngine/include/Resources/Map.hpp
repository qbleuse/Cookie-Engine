#ifndef __MAP_HPP__
#define __MAP_HPP__

#include "ComponentTransform.hpp"
#include "ComponentModel.hpp"
#include "ComponentPhysics.hpp"

#include <array>
#include <string>
#include <algorithm>

struct ID3D11Buffer;

namespace Cookie
{

	namespace Gameplay
	{
		class CGPProducer;
	}

	namespace Resources
	{		

		struct Tile
		{
			int id = 0;
			bool isObstacle = false;
			bool isVisited = false;

			float f = 0;
			float g = 0;
			float h = 0;
			Core::Math::Vec2 pos = { { 0, 0 } };

			std::vector<Tile*> neighbours;
			Tile* parent = nullptr;
		};

		#define MAP_DEFAULT_TILESNB_WIDTH  100
		#define MAP_DEFAULT_TILESNB_HEIGHT 100
		#define MAP_DEFAULT_SCALE_WIDTH    100
		#define MAP_DEFAULT_SCALE_HEIGHT   100

		class Map
		{
		public:
			Core::Math::Vec2 tilesNb      = {{ MAP_DEFAULT_TILESNB_WIDTH, MAP_DEFAULT_TILESNB_HEIGHT }};
			Core::Math::Vec2 tilesSize    = {{ 0, 0 }};
			std::vector<Tile> tiles;

			ECS::ComponentTransform trs;
			ECS::ComponentModel	    model;
			ECS::ComponentPhysics	physic;


			Map();
			~Map() {}

			void InitTiles();
			void ScaleHasChanged();
			void TileNbHasChanged();

			int GetTileIndex(Core::Math::Vec2& mousePos);
			int GetTileIndex(Core::Math::Vec3& pos);
			Tile& GetTile(Core::Math::Vec2& mousePos);
			Tile& GetTile(Core::Math::Vec3& pos);

			void ClampPosInMap(Core::Math::Vec2& posToClamp, Core::Math::Vec2& buildingNbOfTiles);
			void ClampPosInMap(Core::Math::Vec3& posToClamp);
			void ClampPosInMapWithScale(ECS::ComponentTransform& trsToClamp);
			void ClampPosOutsideObstacleTile(ECS::ComponentTransform& trsToClamp);

			Core::Math::Vec2 GetCenterOfBuilding(Core::Math::Vec2& mousePos, Core::Math::Vec2& buildingNbOfTiles);
			Core::Math::Vec3 GetCenterOfBuilding(Core::Math::Vec3& mousePos, Core::Math::Vec2& buildingNbOfTiles);

			bool isBuildingValid(int indexTopLeft, Core::Math::Vec2& tileSize);
			void FillOccupiedTiles(int indexTopLeft, Core::Math::Vec2& tileSize, std::vector<Tile*>& vectorOfOccupiedTiles);

			bool ApplyPathfinding(Tile& tileStart, Tile& tileEnd);

		};


	}
}

#endif // !__MAP_HPP__
