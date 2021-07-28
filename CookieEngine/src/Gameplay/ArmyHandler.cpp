#include "Gameplay/ArmyHandler.hpp"


using namespace Cookie::ECS;
using namespace Cookie::Gameplay;


void ArmyHandler::Debug()
{
	for (int i = 0; i < livingArmies; ++i)
	{
		std::cout << " name : "                 << armies[i].name                 << "\n"
			      << " income primary : "       << armies[i].income.primary       << "\n"
				  << " income secondary : "     << armies[i].income.secondary     << "\n"
				  << " income supplyCurrent : " << armies[i].income.supplyCurrent << "\n"
				  << " income supplyMax : "     << armies[i].income.supplyMax     << "\n"
			      << " nb of workers : "        << armies[i].workers.size()       << "\n"
			      << " nb of units : "          << armies[i].units.size()         << "\n"
			      << " nb of buildings : "      << armies[i].buildings.size()     << "\n\n";
	}
}

void ArmyHandler::UpdateArmyCoordinators(Resources::Map& map)
{
	for (int i = 0; i < armiesCoordinator.size(); ++i)
	{
		armiesCoordinator[i].Analysis();
		armiesCoordinator[i].ResourceAllocation(map);
	}
}

Army* ArmyHandler::GetArmy(E_ARMY_NAME armyName)
{
	for (int i = 0; i < livingArmies; ++i)
		if (armies[i].name == armyName)
			return &armies[i];

	return nullptr;
}
ArmyCoordinator* ArmyHandler::GetArmyCoordinator(E_ARMY_NAME armyName)
{
	for (int i = 0; i < armiesCoordinator.size(); ++i)
		if (armiesCoordinator[i].army->name == armyName)
			return &armiesCoordinator[i];

	return nullptr;
}

void ArmyHandler::AddArmy(E_ARMY_NAME armyName)
{
	if (livingArmies >= MAX_ARMIES)
	{
		CDebug.Warning("Too many armies existing.");
		return;
	}

	armies[livingArmies].name = armyName;
	livingArmies++;
}
void ArmyHandler::RemoveArmy(Army& army)
{
	if (livingArmies <= 0)
	{
		CDebug.Warning("No armies to remove.");
		return;
	}

	livingArmies--;
	Army& lastArmy = armies[livingArmies];
	ArmyCoordinator* lastArmyCoordinator = GetArmyCoordinator(lastArmy.name);

	army = lastArmy;
	lastArmy.Reset();

	if (lastArmyCoordinator)
		lastArmyCoordinator->army = &army;

}

void ArmyHandler::AddArmyCoordinator(E_ARMY_NAME armyName, AIBehavior* aiBehavior)
{
	//quit if there is already an ArmyCoordinator
	if (GetArmyCoordinator(armyName))
		return;

	Army* army = GetArmy(armyName);

	if(army)
		armiesCoordinator.push_back(ArmyCoordinator{army, aiBehavior });
}
void ArmyHandler::RemoveArmyCoordinator(E_ARMY_NAME armyName)
{
	for (int i = 0; i < armiesCoordinator.size(); ++i)
		if (armiesCoordinator[i].army->name == armyName)
		{
			armiesCoordinator.erase(armiesCoordinator.begin() + i);
			return;
		}
}

void ArmyHandler::AddElementToArmy(ComponentGameplay* element)
{
	Army* army = GetArmy(element->teamName);

	if(army)
		AddElementToArmy(*army, element);
	else
	{
		AddArmy(element->teamName);
		AddElementToArmy(armies[livingArmies - 1], element);
	}


}
void ArmyHandler::AddElementToArmy(Army& army, ComponentGameplay* element)
{
	ArmyCoordinator* armyCoordinator = GetArmyCoordinator(army.name);

	switch (element->type)
	{
	case E_ARMY_TYPE::E_WORKER:
		element->componentWorker.income = &(army.income);
		army.workers.push_back(element);
		if (armyCoordinator)
			armyCoordinator->nbOfWorkerInProduction--;
		break;

	case E_ARMY_TYPE::E_UNIT:
		army.units.push_back(element);
		if (armyCoordinator)
			armyCoordinator->nbOfUnitInProduction--;
		break;

	case E_ARMY_TYPE::E_BUILDING:
		element->componentProducer.income = &(army.income);
		army.income.supplyMax += element->componentProducer.supplyGiven;
		army.buildings.push_back(element);
		if (armyCoordinator)
			armyCoordinator->nbOfBuildingInProduction--;
		break;

	default:
		break;
	}
}

void ArmyHandler::RemoveElementFromArmy(ComponentGameplay* element, std::string entityName)
{
	Army* army = GetArmy(element->teamName);
	
	if(army)
		RemoveElementFromArmy(*army, element, entityName);
}
void ArmyHandler::RemoveElementFromArmy(Army& army, ComponentGameplay* element, std::string entityName)
{
	ArmyCoordinator* armyCoordinator = GetArmyCoordinator(army.name);

	switch (element->type)
	{
	case E_ARMY_TYPE::E_WORKER:
		RemoveElementFromVector(army.workers, element);
		break;

	case E_ARMY_TYPE::E_UNIT:
		RemoveElementFromVector(army.units, element);
		break;

	case E_ARMY_TYPE::E_BUILDING:
		army.income.supplyMax -= element->componentProducer.supplyGiven;
		if (armyCoordinator)
			armyCoordinator->stepGoals.listOfBuildings.push_back(entityName);
		RemoveElementFromVector(army.buildings, element);
		break;

	default:
		break;
	}

	RemoveArmyIfEmpty(army);
}
void ArmyHandler::RemoveElementFromVector(std::vector<ComponentGameplay*>& vector, ComponentGameplay* element)
{
	for (int i = 0; i < vector.size(); ++i)
		if (vector[i] == element)
		{
			vector.erase(vector.begin() + i);
			return;
		}
}

void ArmyHandler::RemoveArmyIfEmpty(Army& army)
{
	//if there is still an entity in this army
	if (!army.workers.empty() ||
		!army.units.empty()   ||
		!army.buildings.empty()  )
		return;


	//RemoveArmyCoordinator if existing
	RemoveArmyCoordinator(army.name);

	//Remove Army
	RemoveArmy(army);

	//Check if player Win or Lose
	if (livingArmies == 1)
	{
		if (armies[0].name == E_ARMY_NAME::E_PLAYER)
			PlayerWin();
		else
			PlayerLose();
	}


}
void ArmyHandler::PlayerWin()
{
	std::cout << "PLAYER WIN\n";
}
void ArmyHandler::PlayerLose()
{
	std::cout << "PLAYER LOSE\n";
}