#ifndef __COMPONENT_SCRIPT_HPP__
#define __COMPONENT_SCRIPT_HPP__

#include "Resources/Script.hpp"

#include <vector>
#include <memory>

namespace Cookie
{
	namespace ECS
	{
		class ComponentScript
		{
		
		public:
			std::vector<Resources::ScriptObject> scripts;

			void ToDefault();

			void Start();
			void Update();
		};
	}
}

#endif // !__COMPONENT_SCRIPT_HPP__
