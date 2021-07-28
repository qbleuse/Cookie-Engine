#include "ResourcesManager.hpp"
#include "Resources/Prefab.hpp"
#include "Resources/Map.hpp"
#include "SoundManager.hpp"
#include "ParticlesHandler.hpp"
#include "Core/Primitives.hpp"
#include "Render/DebugRenderer.hpp"
#include "Debug.hpp"

#include "ECS/EntityHandler.hpp"
#include "ECS/ComponentHandler.hpp"
#include "ECS/Coordinator.hpp"

#include "Gameplay/ArmyHandler.hpp"

#include <assert.h>

using namespace Cookie::Resources;
using namespace Cookie::Resources::Particles;
using namespace Cookie::Render;
using namespace Cookie::Gameplay;
using namespace Cookie::Core::Math;
using namespace Cookie::ECS;



//Entity
Entity* Coordinator::AddEntity(const int signature, std::string name)
{
	if (entityHandler->livingEntities >= MAX_ENTITIES)
	{
		CDebug.Warning("Too many entities existing.");
		return nullptr;
	}

	Entity& newEntity = entityHandler->entities[entityHandler->livingEntities];
	entityHandler->livingEntities++;
	newEntity.name = name;
	newEntity.namePrefab = "NONE";

	if (CheckSignature(signature, C_SIGNATURE::TRANSFORM))
		componentHandler->AddComponent(newEntity, C_SIGNATURE::TRANSFORM);
	if (CheckSignature(signature, C_SIGNATURE::MODEL))
		componentHandler->AddComponent(newEntity, C_SIGNATURE::MODEL);
	if (CheckSignature(signature, C_SIGNATURE::PHYSICS))
		componentHandler->AddComponent(newEntity, C_SIGNATURE::PHYSICS);
	if (CheckSignature(signature, C_SIGNATURE::SCRIPT))
		componentHandler->AddComponent(newEntity, C_SIGNATURE::SCRIPT);
	if (CheckSignature(signature, C_SIGNATURE::GAMEPLAY))
		componentHandler->AddComponent(newEntity, C_SIGNATURE::GAMEPLAY);
	if (CheckSignature(signature, C_SIGNATURE::FX))
		componentHandler->AddComponent(newEntity, C_SIGNATURE::FX);

	return &newEntity;
}
Entity* Coordinator::AddEntity(const Resources::Prefab* const & prefab, E_ARMY_NAME teamName)
{
	if (entityHandler->livingEntities >= MAX_ENTITIES || !prefab)
	{
		CDebug.Warning("Too many entities existing or prefab null");
		return nullptr;
	}

	Entity& newEntity = entityHandler->entities[entityHandler->livingEntities];
	entityHandler->livingEntities++;
	newEntity.name = prefab->name;
	newEntity.namePrefab = prefab->name;
	newEntity.signature = prefab->signature;


	if (CheckSignature(newEntity.signature, C_SIGNATURE::TRANSFORM))
	{
		ComponentTransform& trs = componentHandler->GetComponentTransform(newEntity.id);
		trs = prefab->transform;
		trs.modelptr = &componentHandler->GetComponentModel(newEntity.id);
	}
	if (CheckSignature(newEntity.signature, C_SIGNATURE::MODEL))
		componentHandler->GetComponentModel(newEntity.id) = prefab->model;
	if (CheckSignature(newEntity.signature, C_SIGNATURE::PHYSICS))
		componentHandler->GetComponentPhysics(newEntity.id) = prefab->physics;
	if (CheckSignature(newEntity.signature, C_SIGNATURE::SCRIPT))
		componentHandler->GetComponentScript(newEntity.id) = prefab->script;
	if (CheckSignature(newEntity.signature, C_SIGNATURE::GAMEPLAY))
	{
		ComponentGameplay& gameplay = componentHandler->GetComponentGameplay(newEntity.id);
		gameplay = prefab->gameplay;
		gameplay.teamName = teamName;

		ComponentTransform* trsPtr = &componentHandler->GetComponentTransform(newEntity.id);
		gameplay.trs				   = trsPtr;
		gameplay.componentLive.trs     = trsPtr;
		gameplay.componentAttack.trs   = trsPtr;
		gameplay.componentMove.trs     = trsPtr;
		gameplay.componentProducer.trs = trsPtr;

		if(gameplay.teamName != E_ARMY_NAME::E_DEFAULT_NAME)
			armyHandler->AddElementToArmy(&gameplay);
	}

	return &newEntity;
}
void Coordinator::RemoveEntity(Entity& entity)
{
	if (entityHandler->livingEntities <= 0)
	{
		CDebug.Warning("No Entity to remove");
		return;
	}

	//Remove from army
	if (entity.signature & C_SIGNATURE::GAMEPLAY)
		armyHandler->RemoveElementFromArmy(&componentHandler->GetComponentGameplay(entity.id), entity.name);

	//Reset Components
	componentHandler->SubComponentToDefault(entity.signature, entity.id);
	//Reset Entity
	entity.ToDefault();

	//Switch the removed one with the last alive
	entityHandler->livingEntities--;
	entity.Swap(entityHandler->entities[entityHandler->livingEntities]);
}
bool Coordinator::CheckSignature(const int entitySignature, const int signature)
{
	//true if entitySignature has all the component of the given signature
	return (entitySignature & signature) == signature;
}

//Selection
void Coordinator::SelectEntities(Vec3& selectionQuadStart, Vec3& selectionQuadEnd)
{
	selectedEntities.clear();
	float minX = std::min(selectionQuadStart.x, selectionQuadEnd.x);
	float maxX = std::max(selectionQuadStart.x, selectionQuadEnd.x);
	float minZ = std::min(selectionQuadStart.z, selectionQuadEnd.z);
	float maxZ = std::max(selectionQuadStart.z, selectionQuadEnd.z);

	//push back all entities with corresponding signature and inside the selection quad
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).teamName == E_ARMY_NAME::E_PLAYER)
		{
			Vec3& entityPos = componentHandler->GetComponentTransform(entityHandler->entities[i].id).pos;
			if (minX <= entityPos.x && entityPos.x <= maxX &&
				minZ <= entityPos.z && entityPos.z <= maxZ)
				selectedEntities.push_back(&entityHandler->entities[i]);
		}
}
Entity* Coordinator::GetClosestEntity(Vec3& pos, E_ARMY_NAME teamName, int minimumGameplaySignatureWanted)
{
	float   minimumDistance {INFINITY};
	Entity* entityToReturn  {nullptr};

	//Get closest Entity with minimumGameplaySignatureWanted
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).teamName == teamName && 
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, minimumGameplaySignatureWanted))
		{
			ComponentTransform& trs = componentHandler->GetComponentTransform(entityHandler->entities[i].id);
			float possibleNewDistance = (trs.pos - pos).Length();

			if (possibleNewDistance < minimumDistance)
			{
				minimumDistance = possibleNewDistance;
				entityToReturn = &entityHandler->entities[i];
			}
		}

	return entityToReturn;
}
Entity* Coordinator::GetClosestSelectableEntity(Core::Math::Vec3& pos, E_ARMY_NAME teamName, int minimumGameplaySignatureWanted)
{
	//Used when the selection Quad is too small, we check if the user click on a unit

	Entity* entityToReturn = GetClosestEntity(pos, teamName, minimumGameplaySignatureWanted);

	//Check if pos exceed scales
	if (entityToReturn)
	{
		ComponentTransform& trs = componentHandler->GetComponentTransform(entityToReturn->id);

		if (pos.x < trs.pos.x - trs.scale.x / 2 || trs.pos.x + trs.scale.x / 2 < pos.x ||
			pos.z < trs.pos.z - trs.scale.z / 2 || trs.pos.z + trs.scale.z / 2 < pos.z)
			entityToReturn = nullptr;
	}

	return entityToReturn;
}
Entity* Coordinator::GetClosestFreeResourceEntity(Core::Math::Vec3& pos)
{
	//Same logic as "GetClosestEntity" but with additionnal specific conditions

	float   minimumDistance{ INFINITY };
	Entity* entityToReturn{ nullptr };

	//Check if the entity meet all conditions and return the closest
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY))
		{
			ComponentGameplay& gameplay = componentHandler->GetComponentGameplay(entityHandler->entities[i].id);

			if (CheckSignature(gameplay.signatureGameplay, CGP_SIGNATURE::RESOURCE) &&
				gameplay.componentResource.nbOfWorkerOnIt < MAX_WORKER_PER_RESOURCE &&
				gameplay.componentResource.resourceReserve > 0)
			{
				ComponentTransform& trs = componentHandler->GetComponentTransform(entityHandler->entities[i].id);
				float possibleNewDistance = (trs.pos - pos).Length();

				if (possibleNewDistance < minimumDistance)
				{
					minimumDistance = possibleNewDistance;
					entityToReturn = &entityHandler->entities[i];
				}
			}
		}

	//Avoid having workers going too far (or even in an enemy base)
	if (minimumDistance > MAX_RESOURCE_DISTANCE_FROM_BASE)
		return nullptr;

	return entityToReturn;
}


//Primary Component
/*
void Coordinator::ApplySystemPhysics(float factor)
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::PHYSICS))
			System::SystemPhysics(componentHandler->GetComponentPhysics(entityHandler->entities[i].id), factor);
}*/
//void Coordinator::ApplyDraw(const Core::Math::Mat4& viewProj, ID3D11Buffer* CBuffer)
//{
//	for (int i = 0; i < entityHandler->livingEntities; ++i)
//		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL))
//			System::SystemDraw(componentHandler->GetComponentTransform(entityHandler->entities[i].id),
//				componentHandler->GetComponentModel(entityHandler->entities[i].id), viewProj, CBuffer);
//}
void Coordinator::ApplyScriptStart()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::SCRIPT))
			componentHandler->GetComponentScript(entityHandler->entities[i].id).Start();
}
void Coordinator::ApplyScriptUpdate()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::SCRIPT))
			componentHandler->GetComponentScript(entityHandler->entities[i].id).Update();
}
void Coordinator::ApplyGameplay(Map& map)
{
	UpdateCGPProducer(map);
	UpdateCGPWorker(map);
	UpdateCGPMove(map);
	UpdateCGPAttack();
}
void Coordinator::ApplyRemoveUnnecessaryEntities()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
	{
		if (entityHandler->entities[i].needToBeRemoved)
		{
			RemoveEntity(entityHandler->entities[i]);
			i = std::max(i - 1, 0);
		}
		else if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::GAMEPLAY) &&
				 CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::LIVE))
		{
			CGPLive& life = componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentLive;

			if (life.lifeCurrent <= 0)
			{
				if (life.sfxDeath)
					SoundManager::PlayMusic3D(life.sfxDeath, life.trs->pos);
				if (life.vfxDeath)
				{
					Vec3 vfxPos = life.trs->pos;
					vfxPos.y -= life.trs->scale.y * std::abs(life.trs->modelptr->mesh->AABBMin.y);
					ParticlesHandler::CreateParticlesWithPrefab(vfxPos, life.vfxDeath);
				}

				RemoveEntity(entityHandler->entities[i]);
				i = std::max(i - 1, 0);
			}
		}
	
	}
}
void Coordinator::ApplyComputeTrs()
{
	//Used to compute the matrix TRS of each entities at max once per frame

	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM))
		{
			ComponentTransform& trs = componentHandler->GetComponentTransform(entityHandler->entities[i].id);
			
			if (trs.trsHasChanged)
			{
				trs.trsHasChanged = false;
				trs.ComputeTRS();
			}
		}
}


//CGP_Producer
void Coordinator::UpdateCGPProducer(Map& map)
{
	ApplyGameplayUpdateCountdownProducer(map);
}
void Coordinator::ApplyGameplayUpdateCountdownProducer(Map& map)
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::PRODUCER))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentProducer.UpdateCountdown(map, *this, entityHandler->entities[i].id);
}

//CGP_Worker
void Coordinator::UpdateCGPWorker(Map& map)
{
	ApplyGameplayUpdateWorker(map);
}
void Coordinator::ApplyGameplayUpdateWorker(Map& map)
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::WORKER))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentWorker.Update(map , *this, entityHandler->entities[i].id);
}

//CGP_Move
void Coordinator::UpdateCGPMove(Map& map)
{
	ApplyGameplayUpdatePushedCooldown(map);
	ApplyGameplayUpdateReachGoalCooldown();

	ApplyGameplayMoveTowardWaypoint();
	
	ApplyGameplayPosPrediction();
	ApplyGameplayResolveCollision(map);
	
	//ApplyGameplayDrawPath(debug);
}
void Coordinator::ApplyGameplayUpdatePushedCooldown(Map& map)
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::MOVE))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentMove.UpdatePushedCooldown(map);
}
void Coordinator::ApplyGameplayUpdateReachGoalCooldown()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::MOVE))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentMove.UpdateReachGoalCooldown();
}
void Coordinator::ApplyGameplayMoveTowardWaypoint()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::MOVE))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentMove.MoveTowardWaypoint();
}
void Coordinator::ApplyGameplayPosPrediction()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::MOVE))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentMove.PositionPrediction();
}
void Coordinator::ApplyGameplayResolveCollision(Map& map)
{
	std::vector<CGPMove*> allEntitiespossible;
	std::vector<CGPMove*> entitiesToCheck;

	//Fill Vectors for easier use later
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::MOVE))
		{
			CGPMove& cgpMoveSelf = componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentMove;
			allEntitiespossible.push_back(&cgpMoveSelf);
			entitiesToCheck.push_back(&cgpMoveSelf);
		}

	//Recursive Colision
	//for now had a max because in some case it loop forever, will be resolved later on
	int counter = 0;
	while (!entitiesToCheck.empty() && counter < allEntitiespossible.size() * 3)
	{
		counter++;

		for (int i = 0; i < allEntitiespossible.size(); ++i)
		{
			//avoid check with self
			if (entitiesToCheck[0] == allEntitiespossible[i])
				continue;

			Vec3 otherInitialPos = allEntitiespossible[i]->trs->pos;

			//if the two circles collide
			if ((entitiesToCheck[0]->trs->pos - allEntitiespossible[i]->trs->pos).Length() < entitiesToCheck[0]->trs->radius + allEntitiespossible[i]->trs->radius)
				entitiesToCheck[0]->ResolveColision(*allEntitiespossible[i], map);

			//if the other unit was pushed add it back to the entities to check
			if (otherInitialPos != allEntitiespossible[i]->trs->pos)
				entitiesToCheck.emplace(entitiesToCheck.begin() + 1, allEntitiespossible[i]);
		}

		//Remove the checked entity to test next
		entitiesToCheck.erase(entitiesToCheck.begin());
	}

}
void Coordinator::ApplyGameplayDrawPath(DebugRenderer& debug)
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::MOVE))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentMove.DrawPath(debug);
}

//CGP_Attack
void Coordinator::UpdateCGPAttack()
{
	ApplyGameplayCheckEnemyInRange();
	ApplyGameplayAttack();
}
void Coordinator::ApplyGameplayCheckEnemyInRange()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::ATTACK))
		{
			ComponentGameplay& gameplay = componentHandler->GetComponentGameplay(entityHandler->entities[i].id);
			CGPAttack& cgpAttack = gameplay.componentAttack;
			ComponentTransform& trs = componentHandler->GetComponentTransform(entityHandler->entities[i].id);
			cgpAttack.target = nullptr;
			float smallestDist = cgpAttack.attackRange;

			for (int j = 0; j < entityHandler->livingEntities; ++j)
				if (i != j &&
					CheckSignature(entityHandler->entities[j].signature, C_SIGNATURE::TRANSFORM + C_SIGNATURE::GAMEPLAY) &&
					gameplay.teamName != componentHandler->GetComponentGameplay(entityHandler->entities[j].id).teamName &&
					CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[j].id).signatureGameplay, CGP_SIGNATURE::LIVE))
				{
					ComponentTransform& otherTrs = componentHandler->GetComponentTransform(entityHandler->entities[j].id);
					float possibleNewDist = Vec2{ {trs.pos.x - otherTrs.pos.x , trs.pos.z - otherTrs.pos.z } }.Length() - trs.radius - otherTrs.radius;

					if (possibleNewDist < smallestDist )
					{
						smallestDist = possibleNewDist;
						cgpAttack.target = &componentHandler->GetComponentGameplay(entityHandler->entities[j].id).componentLive;
					}
				}

			if (CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::MOVE))
			{
				CGPMove& cgpMove = componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentMove;

				//stop mvt
				if (cgpMove.state == CGPMOVE_STATE::E_MOVING && cgpAttack.target != nullptr)
					cgpMove.state = CGPMOVE_STATE::E_WAITING;

				//resume mvt
				if (cgpMove.state == CGPMOVE_STATE::E_WAITING && cgpAttack.target == nullptr)
					cgpMove.state = CGPMOVE_STATE::E_MOVING;
			}
		}
}
void Coordinator::ApplyGameplayAttack()
{
	for (int i = 0; i < entityHandler->livingEntities; ++i)
		if (CheckSignature(entityHandler->entities[i].signature, C_SIGNATURE::GAMEPLAY) && 
			CheckSignature(componentHandler->GetComponentGameplay(entityHandler->entities[i].id).signatureGameplay, CGP_SIGNATURE::ATTACK))
			componentHandler->GetComponentGameplay(entityHandler->entities[i].id).componentAttack.Attack();
}

