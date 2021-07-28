#ifndef __ENTITY_HANDLER_HPP__
#define __ENTITY_HANDLER_HPP__


#include <vector>
#include <string>

namespace Cookie
{
	namespace ECS
	{
		#define MAX_ENTITIES 1000

		class Entity
		{
		public:
			unsigned int		id				  {0};
			int                 signature		  {0}; 
			std::string			name			  {"No Name"};
			std::string			namePrefab		  {"No Name"};
			std::string			tag				  {"No Tag"};
			bool				needToBeRemoved	  {false};

			Entity(unsigned int _id);
			Entity(unsigned int _id, int _signature, std::string _name, std::string _namePrefab);
			~Entity() {};

			void Swap(Entity& entity);

			void ToDefault();
		};

		class EntityHandler
		{
		public:
			std::vector<Entity> entities;
			unsigned int        livingEntities {0};

			EntityHandler();
			~EntityHandler() {};

			void Debug();
		};

	}
}

#endif