#include "ECS/EntityHandler.hpp"
#include <iostream>

using namespace Cookie::ECS;



//////////////////ENTITY/////////////

Entity::Entity(unsigned int _id) : id{ _id } {}
Entity::Entity(unsigned int _id, int _signature, std::string _name, std::string _namePrefab) : id{ _id }, signature{ _signature }, name{ _name }, namePrefab{ _namePrefab } {}


void Entity::Swap(Entity& entity)
{
	Entity temp{ entity };
	entity = *this;
	*this = temp;
}


void Entity::ToDefault()
{
	signature       = 0;
	name            = "No Name";
	namePrefab      = "No Name";
	tag             = "No Tag";
	needToBeRemoved = false;
}

///////////////ENTITYHANDLER/////////////

EntityHandler::EntityHandler()
{
	for (unsigned int id = 0; id < MAX_ENTITIES; ++id)
		entities.push_back(Entity(id));
}

void EntityHandler::Debug()
{
	for (int i = 0; i < livingEntities; ++i)
	{
		std::cout << " index : "      << i                      << "\n"
				  << " id : "		  << entities[i].id			<< "\n"
			      << " signature : "  << entities[i].signature  << "\n"
				  << " name : "		  << entities[i].name		<< "\n"
				  << " tag : "		  << entities[i].tag		<< "\n"
				  << " namePrefab : " << entities[i].namePrefab << "\n\n";
	}
}