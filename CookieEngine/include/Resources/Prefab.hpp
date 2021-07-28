#ifndef __PREFAB_HPP__
#define __PREFAB_HPP__

#include <string>
#include "ECS/ComponentTransform.hpp"
#include "ECS/ComponentModel.hpp"
#include "ECS/ComponentPhysics.hpp"
#include "ECS/ComponentScript.hpp"
#include "ECS/ComponentGameplay.hpp"

namespace reactphysics3d
{
	class RigidBody;
	class Collider;
}

namespace Cookie
{
	namespace Core
	{
		namespace Math
		{
			union Vec3;
		}
	}

	namespace Resources
	{
		class Prefab
		{
		public:
			std::string									name		= "No Name";
			int											signature = 0;

			Cookie::ECS::ComponentTransform				transform;
			Cookie::ECS::ComponentModel					model;
			Cookie::ECS::ComponentPhysics				physics;
			Cookie::ECS::ComponentScript				script;
			Cookie::ECS::ComponentGameplay				gameplay;

			std::string									filepath	= "Null";

			Prefab() {}
			Prefab(const std::string& _name) : name{ _name }  {}
		};
	}
}


#endif 