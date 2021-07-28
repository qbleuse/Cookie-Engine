#ifndef __COORDINATOR_HPP__
#define __COORDINATOR_HPP__

#include "Core/Math/Mat4.hpp"
#include "ComponentGameplay.hpp" //for E_ARMY_NAME
#include <vector>

namespace Cookie
{
	namespace Gameplay
	{
		class ArmyHandler;
	}

	namespace Resources
	{
		class ResourcesManager;
		class Map;
		class Prefab;
	}
	namespace Render
	{
		class DebugRenderer;
	}


	namespace ECS
	{
		class Entity;
		class EntityHandler;
		class ComponentHandler;

		class Coordinator
		{
		public:
			EntityHandler*         entityHandler    {nullptr};
			ComponentHandler*      componentHandler {nullptr};
			Gameplay::ArmyHandler* armyHandler      {nullptr};
			std::vector<Entity*>   selectedEntities;

			Coordinator() {}
			~Coordinator() {}

			//Entity
			Entity* AddEntity(const int signature, std::string name = std::string("No Name"));
			Entity* AddEntity(const Resources::Prefab* const & prefab, Gameplay::E_ARMY_NAME teamName = Gameplay::E_ARMY_NAME::E_DEFAULT_NAME);
			void RemoveEntity(Entity& entity);
			bool CheckSignature(const int entitySignature, const int signature);
			
			//Selection
			void SelectEntities(Core::Math::Vec3& selectionQuadStart, Core::Math::Vec3& selectionQuadEnd);
			Entity* GetClosestEntity(Core::Math::Vec3& pos, Gameplay::E_ARMY_NAME teamName, int minimumGameplaySignatureWanted = 0);
			Entity* GetClosestSelectableEntity(Core::Math::Vec3& pos, Gameplay::E_ARMY_NAME teamName, int minimumGameplaySignatureWanted = 0);
			Entity* GetClosestFreeResourceEntity(Core::Math::Vec3& pos);

			//Primary Component
			//void ApplySystemPhysics(float factor);
			//void ApplyDraw(const Core::Math::Mat4& viewProj);
			void ApplyScriptStart();
			void ApplyScriptUpdate();
			void ApplyGameplay(Resources::Map& map);
			void ApplyRemoveUnnecessaryEntities();
			void ApplyComputeTrs();


			//CGP_Producer
			void UpdateCGPProducer(Resources::Map& map);
			void ApplyGameplayUpdateCountdownProducer(Resources::Map& map);

			//CGP_Worker
			void UpdateCGPWorker(Resources::Map& map);
			void ApplyGameplayUpdateWorker(Resources::Map& map);

			//CGP_Move
			void UpdateCGPMove(Resources::Map& map);
			void ApplyGameplayUpdatePushedCooldown(Resources::Map& map);
			void ApplyGameplayUpdateReachGoalCooldown();
			void ApplyGameplayMoveTowardWaypoint();
			void ApplyGameplayPosPrediction();
			void ApplyGameplayResolveCollision(Resources::Map& map);
			void ApplyGameplayDrawPath(Render::DebugRenderer& debug);

			//CGP_Attack
			void UpdateCGPAttack();
			void ApplyGameplayCheckEnemyInRange();
			void ApplyGameplayAttack();

		};

	}
}

#endif
