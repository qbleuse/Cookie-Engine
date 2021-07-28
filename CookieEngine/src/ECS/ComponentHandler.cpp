#include "ECS/ComponentHandler.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Physics/PhysicsHandle.hpp"
#include "Resources/Prefab.hpp"

using namespace Cookie::ECS;

/*============ PHYSICS ============*/

void ComponentHandler::InitComponentPhysic(Entity& entity)
{
	if (entity.signature & C_SIGNATURE::TRANSFORM)
		componentPhysics[entity.id].Set(componentTransforms[entity.id]);

	componentPhysics[entity.id].physBody = Physics::PhysicsHandle::physSim.worldSim->createRigidBody(componentPhysics[entity.id].physTransform);
}
