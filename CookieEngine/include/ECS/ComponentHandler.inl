#ifndef __COMPONENT_HANDLER_INL__
#define __COMPONENT_HANDLER_INL__

#include "Debug.hpp"

namespace Cookie
{
	namespace ECS
	{
		inline ComponentHandler::ComponentHandler()
		{
			for (int i = 0; i < MAX_ENTITIES; ++i)
			{
				ComponentTransform* trsPtr = &componentTransforms[i];
				trsPtr->modelptr = &componentModels[i];
				componentGameplays[i].trs = trsPtr;
				componentGameplays[i].componentLive.trs = trsPtr;
				componentGameplays[i].componentAttack.trs = trsPtr;
				componentGameplays[i].componentMove.trs = trsPtr;
				componentGameplays[i].componentProducer.trs = trsPtr;

			}
		}

		inline void ComponentHandler::AddComponent(Entity& entity, int signature) noexcept
		{
			if (entity.signature & signature)
			{
				CDebug.Warning("Try to Add Component already present");
				return;
			}

			entity.signature += signature;
		}
		inline void ComponentHandler::RemoveComponent(Entity& entity, int signature) noexcept
		{
			if (entity.signature & signature)
			{
				SubComponentToDefault(signature, entity.id);
				entity.signature -= signature;
				return;
			}

			CDebug.Warning("Try to Remove a Component not present");
		}
		inline void ComponentHandler::SubComponentToDefault(int signature, int entityId)noexcept
		{
			if (signature & C_SIGNATURE::TRANSFORM)
				GetComponentTransform(entityId).ToDefault();
			if (signature & C_SIGNATURE::MODEL)
				GetComponentModel(entityId).ToDefault();
			if (signature & C_SIGNATURE::PHYSICS)
				GetComponentPhysics(entityId).ToDefault();
			if (signature & C_SIGNATURE::SCRIPT)
				GetComponentScript(entityId).ToDefault();
			if (signature & C_SIGNATURE::GAMEPLAY)
				GetComponentGameplay(entityId).ToDefault();
		}


		inline ComponentTransform& ComponentHandler::GetComponentTransform(const unsigned int id) noexcept { return componentTransforms[id]; }
		inline ComponentModel& ComponentHandler::GetComponentModel(const unsigned int id) noexcept         { return componentModels[id]; }
		inline ComponentPhysics& ComponentHandler::GetComponentPhysics(const unsigned int id) noexcept     { return componentPhysics[id]; }
		inline ComponentScript& ComponentHandler::GetComponentScript(const unsigned int id) noexcept       { return componentScripts[id]; }
		inline ComponentGameplay& ComponentHandler::GetComponentGameplay(const unsigned int id) noexcept   { return componentGameplays[id]; }
	}
}


#endif