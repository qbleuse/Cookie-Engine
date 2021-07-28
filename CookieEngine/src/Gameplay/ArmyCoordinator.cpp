#include "Gameplay/ArmyCoordinator.hpp"
#include "ECS/ComponentGameplay.hpp"
#include "Resources/Map.hpp"
#include "Resources/Prefab.hpp"
#include "ArmyHandler.hpp"


using namespace Cookie::Gameplay;
using namespace Cookie::Core::Math;
using namespace Cookie::Resources;
using namespace Cookie::ECS;


void ArmyCoordinator::Analysis()
{
	if (!behavior)
	{
		CDebug.Warning("try to run an AI without giving a Behavior (Check World Settings)");
		return;
	}


	//Economic
	if(army->workers.size() + nbOfWorkerInProduction < stepGoals.nbOfWorker)
		goals.push_back(E_GOALS::E_DEVELOP_WORKER);
	if(stepGoals.listOfBuildings.size() > 0)
		goals.push_back(E_GOALS::E_DEVELOP_BASE);
	if (army->units.size() + nbOfUnitInProduction < stepGoals.nbOfUnits)
		goals.push_back(E_GOALS::E_DEVELOP_ARMY);

	//Military
	//if base under attack
	//goals.push_back(E_GOALS::E_DEFEND);

	//if Reach Goal
	if (army->workers.size() >= stepGoals.nbOfWorker &&
		stepGoals.listOfBuildings.empty() &&
		army->units.size() >= stepGoals.nbOfUnits )
	{
		if (canAttack)
		{
			canAttack = false;
			goals.push_back(E_GOALS::E_ATTACK);
			AddNextStep();
		}
	}
	else
		canAttack = true;

}

void ArmyCoordinator::ResourceAllocation(Map& map)
{
	for (int i = 0; i < goals.size(); ++i)
	{
		switch (goals[i])
		{
		case E_GOALS::E_DEVELOP_WORKER:
			DevelopWorker();
			break;
		case E_GOALS::E_DEVELOP_BASE:
			DevelopBase(map);
			break;

		case E_GOALS::E_DEVELOP_ARMY:
			DevelopArmy();
			break;

		case E_GOALS::E_DEFENSE:
			Defense(map);
			break;

		case E_GOALS::E_ATTACK:
			Attack(map);
			break;

		case E_GOALS::E_RETREAT:
			Retreat();
			break;

		default:
			break;
		}
	}

	goals.clear();
}

void ArmyCoordinator::DevelopWorker()
{
	for (int i = 0; i < army->buildings.size(); ++i)
	{
		CGPProducer& producer = army->buildings[i]->componentProducer;

		for (int j = 0; j < producer.possibleUnits.size(); ++j)
		{
			if (producer.possibleUnits[j]->gameplay.signatureGameplay & CGP_SIGNATURE::WORKER &&
				producer.AddUnitToQueue(j))
			{
				nbOfWorkerInProduction++;
				return;
			}
		}
	}

}
void ArmyCoordinator::DevelopBase(Map& map)
{
	//CLEAN later on 

	for (int i = 0; i < army->workers.size(); ++i)
	{
		CGPWorker& worker = army->workers[i]->componentWorker;

		if(!worker.BuildingInConstruction)
			for (int j = 0; j < worker.possibleBuildings.size(); ++j)
			{
				std::string name = worker.possibleBuildings[j]->name;

				for (int k = 0; k < stepGoals.listOfBuildings.size(); ++k)
					if (name == stepGoals.listOfBuildings[k])
					{
						//Calculate Pos
						Vec3 pos {INFINITY, INFINITY, INFINITY};
						Vec2 newBuildingTileNb = worker.possibleBuildings[j]->gameplay.componentProducer.tileSize;
						for (int l = 0; l < army->buildings.size(); ++l)
						{
							//make a function for pos check later on 

							//Check Right pos
							Vec3 posToCheck = army->buildings[l]->trs->pos + Vec3{(NB_TILES_BETWEEN_BUILDINGS + newBuildingTileNb.x) * map.tilesSize.x + ((int)newBuildingTileNb.x % 2) * map.tilesSize.x / 2, 0, 0};
							posToCheck.y -= army->buildings[l]->trs->scale.y * std::abs(army->buildings[l]->trs->modelptr->mesh->AABBMin.y) - worker.possibleBuildings[j]->transform.scale.y * std::abs(worker.possibleBuildings[j]->model.mesh->AABBMin.y);
							if (!map.GetTile(posToCheck).isObstacle)
							{
								pos = posToCheck;
								break;
								
							}

							//Check Bottom pos
							posToCheck = army->buildings[l]->trs->pos + Vec3{0, 0, (NB_TILES_BETWEEN_BUILDINGS + newBuildingTileNb.y) * map.tilesSize.y + ((int)newBuildingTileNb.y % 2) * map.tilesSize.y / 2 };
							posToCheck.y -= army->buildings[l]->trs->scale.y * std::abs(army->buildings[l]->trs->modelptr->mesh->AABBMin.y) - worker.possibleBuildings[j]->transform.scale.y * std::abs(worker.possibleBuildings[j]->model.mesh->AABBMin.y);
							if (!map.GetTile(posToCheck).isObstacle)
							{
								pos = posToCheck;
								break;
							}

							//Check Right-Bottom pos
							posToCheck = army->buildings[l]->trs->pos + Vec3{ (NB_TILES_BETWEEN_BUILDINGS + newBuildingTileNb.x) * map.tilesSize.x + ((int)newBuildingTileNb.x % 2) * map.tilesSize.x / 2, 0, (NB_TILES_BETWEEN_BUILDINGS + newBuildingTileNb.y) * map.tilesSize.y + ((int)newBuildingTileNb.y % 2) * map.tilesSize.y / 2 };
							posToCheck.y -= army->buildings[l]->trs->scale.y * std::abs(army->buildings[l]->trs->modelptr->mesh->AABBMin.y) - worker.possibleBuildings[j]->transform.scale.y * std::abs(worker.possibleBuildings[j]->model.mesh->AABBMin.y);
							if (!map.GetTile(posToCheck).isObstacle)
							{
								pos = posToCheck;
								break;
							}
						}

						if (pos == Vec3{ INFINITY, INFINITY, INFINITY })
							return;

						if (worker.StartBuilding(map, pos, j))
						{
							stepGoals.listOfBuildings.erase(stepGoals.listOfBuildings.begin() + k);
							nbOfBuildingInProduction++;
						}
						return;
					}

			}
	}
}
void ArmyCoordinator::DevelopArmy()
{
	for (int i = 0; i < army->buildings.size(); ++i)
	{
		CGPProducer& producer = army->buildings[i]->componentProducer;

		for (int j = 0; j < producer.possibleUnits.size(); ++j)
		{
			if (producer.possibleUnits[j]->gameplay.signatureGameplay & CGP_SIGNATURE::ATTACK &&
				producer.AddUnitToQueue(j))
			{
				nbOfUnitInProduction++;
				return;
			}
		}
	}

}
void ArmyCoordinator::Attack(Map& map)
{
	for (int i = 0; i < army->units.size(); ++i)
	{
		ComponentGameplay* gameplay = army->units[i];
		ComponentTransform* trs = gameplay->trs;
		
		if (map.ApplyPathfinding(map.GetTile(trs->pos), map.tiles[map.tiles.size() - 1]))
			gameplay->componentMove.SetPath(map.tiles[map.tiles.size() - 1]);
		else
			std::cout << "No Path Find\n";
	}
}
void ArmyCoordinator::Defense(Map& map)
{
	for (int i = 0; i < army->units.size(); ++i)
	{
		ComponentGameplay* gameplay = army->units[i];
		ComponentTransform* trs = gameplay->trs;

		if (map.ApplyPathfinding(map.GetTile(trs->pos), map.tiles[map.tiles.size() - 1]))
			gameplay->componentMove.SetPath(map.tiles[0]);
		else
			std::cout << "No Path Find\n";
	}
}
void ArmyCoordinator::Retreat()
{
	//std::cout << "AI Retreat\n";
}