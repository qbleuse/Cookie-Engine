#ifndef __COMPONENT_HANDLER_HPP__
#define __COMPONENT_HANDLER_HPP__

#include <array>

#include "ECS/EntityHandler.hpp"
#include "ECS/ComponentTransform.hpp"
#include "ECS/ComponentModel.hpp"
#include "ECS/ComponentPhysics.hpp"
#include "ECS/ComponentScript.hpp"
#include "ECS/ComponentGameplay.hpp"


namespace Cookie
{
	namespace Resources
	{
		class ResourcesManager;
	}

	namespace ECS
	{
		enum  C_SIGNATURE
		{
			EMPTY_C       = 0b000000,
			TRANSFORM     = 0b000001,
			MODEL         = 0b000010,
			PHYSICS	      = 0b000100,
			SCRIPT        = 0b001000,
			GAMEPLAY      = 0b010000,
			FX			  = 0b100000, //not a real Component, used for customization in Inspector
			ALL_C         = 0b111111
		};


		class ComponentHandler
		{
		private:

			std::array<ComponentTransform,	MAX_ENTITIES> componentTransforms;
			std::array<ComponentModel,		MAX_ENTITIES> componentModels;
			std::array<ComponentPhysics,	MAX_ENTITIES> componentPhysics;
			std::array<ComponentScript,		MAX_ENTITIES> componentScripts;
			std::array<ComponentGameplay,   MAX_ENTITIES> componentGameplays;


		public:
			ComponentHandler();
			~ComponentHandler() {}

			inline void AddComponent(Entity& entity, int signature) noexcept;
			inline void RemoveComponent(Entity& entity, int signature) noexcept;
			inline void SubComponentToDefault(int signature, int entityId)noexcept;


			void InitComponentPhysic(Entity& entity);


			inline ComponentTransform& GetComponentTransform (const unsigned int id) noexcept;
			inline ComponentModel&     GetComponentModel     (const unsigned int id) noexcept;
			inline ComponentPhysics&   GetComponentPhysics   (const unsigned int id) noexcept;
			inline ComponentScript&    GetComponentScript    (const unsigned int id) noexcept;
			inline ComponentGameplay&  GetComponentGameplay  (const unsigned int id) noexcept;
		};

	}
}

#include "ECS/ComponentHandler.inl"

#endif
