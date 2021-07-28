#include "Render/D3D11Helper.hpp"
#include "Physics/PhysicsHandle.hpp"
#include "Core/Primitives.hpp"
#include "Render/DebugRenderer.hpp"
#include "Resources/Map.hpp"
#include "Gameplay/CGPProducer.hpp"

#include <list>

using namespace Cookie::Resources;
using namespace Cookie::Core::Math;
using namespace Cookie::ECS;
using namespace Cookie::Gameplay;

Map::Map()
{
	trs.scale = { MAP_DEFAULT_SCALE_WIDTH, 1.0f, MAP_DEFAULT_SCALE_HEIGHT };
	//trs.ComputeTRS();
	//tilesSize = { { trs.scale.x / tilesNb.x, trs.scale.z / tilesNb.y } };

	//init Component Physic
	//physic.physBody = Physics::PhysicsHandle::physSim->createRigidBody(rp3d::Transform(rp3d::Vector3(0.0, 0.0, 0.0), rp3d::Quaternion::identity()));
	//physic.physBody->setType(rp3d::BodyType::STATIC);
	//physic.AddCubeCollider(trs.scale / 2.f, trs.pos, trs.rot);

	//InitTiles();
}

void Map::InitTiles()
{
	tiles.clear();

	for(int i = 0; i < tilesNb.x * tilesNb.y; i++)
		tiles.push_back(Tile());

	for (int x = 0; x < tilesNb.x; x++)
		for (int y = 0; y < tilesNb.y; y++)
		{
			Tile& currentTile = tiles[x + y * tilesNb.x];
			currentTile.id = x + y * tilesNb.x;
			currentTile.pos = { { x * tilesSize.x + tilesSize.x / 2 - trs.scale.x / 2, y * tilesSize.y + tilesSize.y / 2 - trs.scale.z / 2} };

			//Connect to neighbours
			{
				//adjacent
				if (y > 0)
					currentTile.neighbours.push_back(&tiles[x + (y - 1) * tilesNb.x]);
				if (y < tilesNb.y - 1)
					currentTile.neighbours.push_back(&tiles[x + (y + 1) * tilesNb.x]);
				if (x > 0)
					currentTile.neighbours.push_back(&tiles[(x - 1) + y * tilesNb.x]);
				if (x < tilesNb.x - 1)
					currentTile.neighbours.push_back(&tiles[(x + 1) + y * tilesNb.x]);
				
				//diagonal
				if (x > 0 && y > 0)
					currentTile.neighbours.push_back(&tiles[(x - 1) + (y - 1) * tilesNb.x]);
				if (x > 0 && y < tilesNb.y - 1)
					currentTile.neighbours.push_back(&tiles[(x - 1) + (y + 1) * tilesNb.x]);
				if (x < tilesNb.x - 1 && y > 0)
					currentTile.neighbours.push_back(&tiles[(x + 1) + (y - 1) * tilesNb.x]);
				if (x < tilesNb.x - 1 && y < tilesNb.y - 1)
					currentTile.neighbours.push_back(&tiles[(x + 1) + (y + 1) * tilesNb.x]);
			}

		}

}
void Map::ScaleHasChanged()
{
	trs.ComputeTRS();

	TileNbHasChanged();
	
	if (physic.physColliders.size() > 0)
	{
		physic.physBody->removeCollider(physic.physColliders[0]);
		physic.physColliders.clear();
		physic.AddCubeCollider(trs.scale / 2.f, trs.pos, trs.rot);
	}
}
void Map::TileNbHasChanged()
{
	tilesSize = { { trs.scale.x / tilesNb.x, trs.scale.z / tilesNb.y } };
	InitTiles();
}

int  Map::GetTileIndex(Vec2& mousePos)
{
	Vec2 unsignedMousePos{ {mousePos.x + trs.scale.x / 2, mousePos.y + trs.scale.z / 2} };

	return std::clamp(int(unsignedMousePos.x / tilesSize.x), 0, int(tilesNb.x - 1) ) + tilesNb.x * std::clamp(int(unsignedMousePos.y / tilesSize.y), 0, int(tilesNb.y - 1));
}
int  Map::GetTileIndex(Vec3& pos)
{
	//helper, same as GetTileIndex(Vec2) juste use Vec3.z instead of Vec2.y
	Vec2 unsignedPos{ {pos.x + trs.scale.x / 2, pos.z + trs.scale.z / 2} };

	return std::clamp(int(unsignedPos.x / tilesSize.x), 0, int(tilesNb.x - 1)) + tilesNb.x * std::clamp(int(unsignedPos.y / tilesSize.y), 0, int(tilesNb.y - 1));
}
Tile& Map::GetTile(Core::Math::Vec2& mousePos)
{
	return tiles[GetTileIndex(mousePos)];
}
Tile& Map::GetTile(Core::Math::Vec3& pos)
{
	return tiles[GetTileIndex(pos)];
}

void Map::ClampPosInMap(Vec2& posToClamp, Vec2& buildingNbOfTiles)
{
	float minX = -trs.scale.x / 2 + buildingNbOfTiles.x * tilesSize.x / 2;
	float maxX =  trs.scale.x / 2 - buildingNbOfTiles.x * tilesSize.x / 2;
	float minZ = -trs.scale.z / 2 + buildingNbOfTiles.y * tilesSize.y / 2;
	float maxZ =  trs.scale.z / 2 - buildingNbOfTiles.y * tilesSize.y / 2;

	posToClamp = { {std::min(std::max(minX, posToClamp.x), maxX),
				   std::min(std::max(minZ, posToClamp.y), maxZ) } };
}
void Map::ClampPosInMap(Vec3& posToClamp)
{
	float minX = -trs.scale.x / 2;
	float maxX =  trs.scale.x / 2;
	float minZ = -trs.scale.z / 2;
	float maxZ =  trs.scale.z / 2;

	posToClamp  = {std::min(std::max(minX, posToClamp.x), maxX),
				   posToClamp.y,
				   std::min(std::max(minZ, posToClamp.z), maxZ) };
}
void Map::ClampPosInMapWithScale(ComponentTransform& trsToClamp)
{
	float minX = -trs.scale.x / 2 + trsToClamp.scale.x / 2;
	float maxX =  trs.scale.x / 2 - trsToClamp.scale.x / 2;
	float minZ = -trs.scale.z / 2 + trsToClamp.scale.z / 2;
	float maxZ =  trs.scale.z / 2 - trsToClamp.scale.z / 2;

	trsToClamp.pos = {std::min(std::max(minX, trsToClamp.pos.x), maxX),
				      trsToClamp.pos.y,
					  std::min(std::max(minZ, trsToClamp.pos.z), maxZ) };

	trsToClamp.trsHasChanged = true;
}
void Map::ClampPosOutsideObstacleTile(ComponentTransform& trsToClamp)
{
	Tile& tile = GetTile(trsToClamp.pos);

	if (!tile.isObstacle)
		return;

	Vec3 tilePos { tile.pos.x, trsToClamp.pos.y, tile.pos.y };
	Vec3 direction = (trsToClamp.pos - tilePos).Normalize();
	trsToClamp.pos = tilePos + direction * (trsToClamp.scale.x / 2 + tilesSize.x / 2);
	trsToClamp.trsHasChanged = true;
}

Vec2 Map::GetCenterOfBuilding(Vec2& mousePos, Vec2& buildingNbOfTiles)
{
	Vec2 tilePos = tiles[GetTileIndex(mousePos)].pos;

	//if buildingNbOfTiles has peer values add half tileSize for each
	tilePos += { {(int)(buildingNbOfTiles.x + 1) % 2 * tilesSize.x / 2, (int)(buildingNbOfTiles.y + 1) % 2 * tilesSize.y / 2}};

	float leftX = -trs.scale.x / 2 + buildingNbOfTiles.x * tilesSize.x / 2;
	float rightX = trs.scale.x / 2 - buildingNbOfTiles.x * tilesSize.x / 2;
	float leftZ = -trs.scale.z / 2 + buildingNbOfTiles.y * tilesSize.y / 2;
	float rightZ = trs.scale.z / 2 - buildingNbOfTiles.y * tilesSize.y / 2;

	return  { {std::clamp(tilePos.x, (leftX <= rightX) ? leftX : rightX, (leftX <= rightX) ? rightX : leftX),
			 std::clamp(tilePos.y, (leftZ <= rightZ) ? leftZ : rightZ, (leftZ <= rightZ) ? rightZ : leftZ)} };

}
Vec3 Map::GetCenterOfBuilding(Vec3& mousePos, Vec2& buildingNbOfTiles)
{
	Vec2 tilePos = tiles[GetTileIndex(mousePos)].pos;

	//if buildingNbOfTiles has peer values add half tileSize for each
	tilePos += { {(int)(buildingNbOfTiles.x + 1) % 2 * tilesSize.x / 2, (int)(buildingNbOfTiles.y + 1) % 2 * tilesSize.y / 2}};
	
	ClampPosInMap(tilePos, buildingNbOfTiles);

	return {tilePos.x, mousePos.y, tilePos.y};

}

bool Map::isBuildingValid(int indexTopLeft, Vec2& tileSize)
{

	for (int i = 0; i < tileSize.x; ++i)
		for (int j = 0; j < tileSize.y; ++j)
			if (tiles[indexTopLeft + i + tilesNb.x * j].isObstacle)
				return false;

	return true;
}

void Map::FillOccupiedTiles(int indexTopLeft, Vec2& tileSize, std::vector<Tile*>& vectorOfOccupiedTiles)
{
	vectorOfOccupiedTiles.clear();

	for (int i = 0; i < tileSize.x; ++i)
		for (int j = 0; j < tileSize.y; ++j)
		{
			Tile* currentTile = &tiles[indexTopLeft + i + tilesNb.x * j];
			currentTile->isObstacle = true;
			vectorOfOccupiedTiles.push_back(currentTile);
		}
}

bool Map::ApplyPathfinding(Tile& tileStart, Tile& tileEnd)
{
	if (tileEnd.isObstacle)
		return false;
	//if we are already on the tile end return true without doing any calculation
	if (&tileStart == &tileEnd)
	{
		tileEnd.parent = nullptr;
		return true;
	}

	// Set all Tiles to default 
	for (int x = 0; x < tilesNb.x; x++)
		for (int y = 0; y < tilesNb.y; y++)
		{
			Tile& currentTile = tiles[x + y * tilesNb.x];

			currentTile.isVisited = false;
			currentTile.f = INFINITY;
			currentTile.g = INFINITY;
			currentTile.h = INFINITY;
			currentTile.parent = nullptr;
		}

	// Setup starting conditions
	tileStart.g = 0.0f;
	tileStart.h = (tileEnd.pos - tileStart.pos).Length();
	tileStart.f = tileStart.g + tileStart.h;
	std::list<Tile*> listNotTestedTiles;
	listNotTestedTiles.push_back(&tileStart);
	Tile* currentTile = &tileStart;


	while (!listNotTestedTiles.empty() && currentTile != &tileEnd)
	{
		// Sort list of Tiles by lower f
		listNotTestedTiles.sort([](const Tile* firstTile, const Tile* secondTile) { return firstTile->f < secondTile->f; });

		// remove visited Tiles
		while (!listNotTestedTiles.empty() && listNotTestedTiles.front()->isVisited)
			listNotTestedTiles.pop_front();

		// abort if the list is empty
		if (listNotTestedTiles.empty())
			return false;


		currentTile = listNotTestedTiles.front();
		currentTile->isVisited = true;


		// Check each of this node's neighbours...
		for (Tile* currentNeighbour : currentTile->neighbours)
		{
			//Add currentNeighbour to the list only if not visited and not an obstacle
			if (!currentNeighbour->isVisited && !currentNeighbour->isObstacle)
				listNotTestedTiles.push_back(currentNeighbour);

			// Calculate neighbour g value if currentTile become his parent
			float possiblyNewGValue = currentTile->g + (currentTile->pos - currentNeighbour->pos).Length();

			//if it is shorter to have CurrentTile as parent update values of currentNeighbour
			if (possiblyNewGValue < currentNeighbour->g)
			{
				currentNeighbour->parent = currentTile;
				currentNeighbour->g = possiblyNewGValue;
				currentNeighbour->h = (currentNeighbour->pos - tileEnd.pos).Length();
				currentNeighbour->f = currentNeighbour->g + currentNeighbour->h;
			}
		}
	}

	return true;
}
