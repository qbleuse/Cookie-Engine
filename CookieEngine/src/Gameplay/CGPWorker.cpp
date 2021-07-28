#include "Gameplay/CGPWorker.hpp"
#include "Core/Time.hpp"
#include "Resources/Prefab.hpp"
#include "Resources/Map.hpp"
#include "Gameplay/ArmyHandler.hpp"
#include "ECS/Coordinator.hpp"
#include "ECS/ComponentHandler.hpp"


using namespace Cookie::Core::Math;
using namespace Cookie::Gameplay;
using namespace Cookie::ECS;
using namespace Cookie::Resources;


void CGPWorker::Update(Resources::Map& map, Coordinator& coordinator, int selfId)
{

	//isBuilding
	if (constructionCountdown > 0)
	{
		constructionCountdown -= Core::DeltaTime();

		if (constructionCountdown <= 0)
		{
			Entity* newEntity = coordinator.AddEntity(BuildingInConstruction, coordinator.componentHandler->GetComponentGameplay(selfId).teamName);

			coordinator.componentHandler->GetComponentTransform(newEntity->id).pos = posBuilding;
			coordinator.componentHandler->GetComponentGameplay(newEntity->id).componentProducer.occupiedTiles = occupiedTiles;
			occupiedTiles.clear();
			BuildingInConstruction = nullptr;
		}
		else
			return;
	}

	//isHarvesting
	if (harvestCountdown > 0 && !needTostartBuilding)
	{
		harvestCountdown -= Core::DeltaTime();
		if (harvestCountdown <= 0)
		{
			isCarryingResource = true;
			resource->resourceReserve -= (resource->isPrimary ? PRIMARY_PER_HARVEST : SECONDARY_PER_HARVEST);

			if (resource->resourceReserve <= 0)
			{
				Entity* resourceEntity = coordinator.GetClosestFreeResourceEntity(*posResource);

				if (resourceEntity)
					SetResource(coordinator.componentHandler->GetComponentTransform(resourceEntity->id).pos, coordinator.componentHandler->GetComponentGameplay(resourceEntity->id).componentResource);
			}
				
		}
		else
			return;
	}

	//isMoving
	{
		ComponentTransform& trs = coordinator.componentHandler->GetComponentTransform(selfId);
		Core::Math::Vec3* destination = (needTostartBuilding) ? &posBuilding : (isCarryingResource) ? posBase : posResource;
		if (!destination)
			return;
		//avoid mvt on y axis
		Core::Math::Vec3 direction = Vec3{ destination->x - trs.pos.x, 0, destination->z - trs.pos.z };
		trs.pos += direction.Normalize() * (moveSpeed * Core::DeltaTime());
		trs.rot = Mat4::LookAt(Vec3{ 0, 0, 0 }, direction, Vec3{ 0, 1, 0 }).GetEuler();
		trs.trsHasChanged = true;

		//HasReachDestination
		if (direction.Length() < 0.1)
		{
			if (needTostartBuilding)
			{
				needTostartBuilding = false;
				constructionCountdown = BuildingInConstruction->gameplay.cost.timeToProduce;
			}
			else if (isCarryingResource)
			{
				isCarryingResource = false;			
				(resource->isPrimary ? income->primary : income->secondary) += (resource->isPrimary ? PRIMARY_PER_HARVEST : SECONDARY_PER_HARVEST);
			}
			else
				harvestCountdown = TIME_TO_HARVEST;
		}
	}

	
}

void CGPWorker::SetResource(Core::Math::Vec3& resourcePos, CGPResource& resourceCGP)
{
	if (resource)
		resource->nbOfWorkerOnIt--;

	posResource = &resourcePos;
	resource = &resourceCGP;
	resource->nbOfWorkerOnIt++;
}

bool CGPWorker::StartBuilding(Map& map, Vec3& _posBuilding, int indexInPossibleBuildings)
{
	if (indexInPossibleBuildings >= possibleBuildings.size() || needTostartBuilding || constructionCountdown)
		return false;

	Resources::Prefab* const& buildingToAdd = possibleBuildings[indexInPossibleBuildings];

	//later on add Debug.Log depending on what is blocking the process to give player Feedback
	if (buildingToAdd->gameplay.cost.costPrimary > income->primary ||
		buildingToAdd->gameplay.cost.costSecondary > income->secondary)
	{
		std::cout << "not enough Resources to Build\n";
		return false;
	}

	income->primary   -= buildingToAdd->gameplay.cost.costPrimary;
	income->secondary -= buildingToAdd->gameplay.cost.costSecondary;

	posBuilding = _posBuilding;
	BuildingInConstruction = possibleBuildings[indexInPossibleBuildings];
	needTostartBuilding = true;

	Vec3 posTopLeft = posBuilding - buildingToAdd->transform.scale / 2;
	map.FillOccupiedTiles(map.GetTileIndex(posTopLeft), buildingToAdd->gameplay.componentProducer.tileSize, occupiedTiles);
	return true;
}