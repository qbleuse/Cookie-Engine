#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

#include <json.hpp>

using json = nlohmann::json;

namespace Cookie
{
	class Game;

	namespace ECS
	{
		class Coordinator;
		class ComponentPhysics;
		class ComponentHandler;
		class ComponentGameplay;
		class EntityHandler;
	}

	namespace reactphysics3d
	{
		class PhysicsCommon;
	}

	namespace Core::Math
	{
		union Vec4;
	}

	namespace Resources
	{
		class ResourcesManager;
		class Scene;
		class Prefab;
		class Texture;
		class Sound;
		namespace Particles
		{
			class ParticlesSystem;
			class ParticlesPrefab;
		}
	}

	namespace Gameplay
	{
		struct AIBehavior;
	}

	namespace Resources
	{
		namespace Serialization
		{
			namespace Save
			{
				//Save entity
				void ToJson(json& js, const Cookie::ECS::EntityHandler& entity);
				//Save components
				void ToJson(json& js, const Cookie::ECS::EntityHandler& entity, Cookie::ECS::ComponentHandler& component, 
					Cookie::Resources::ResourcesManager& resourcesManager);

				//For each functions we save the elements that we need 
				void SaveScene(Cookie::Resources::Scene& actScene, Cookie::Resources::ResourcesManager& resourcesManager);
				void SavePrefab(const Prefab* const & prefab);
				void SavePhysic(json& js, Cookie::ECS::ComponentPhysics& physics);

				void SaveAllPrefabs(Cookie::Resources::ResourcesManager& resourcesManager);
				void SaveTexture(std::string& name, Cookie::Core::Math::Vec4& color);
				void SaveVolumAndModeMusic(Sound* const & sound);
				void SaveParticles(Cookie::Resources::Particles::ParticlesPrefab& particles);
				void SaveAIBehavior(Cookie::Gameplay::AIBehavior& aiBehavior);
			}

			namespace Load
			{
				//Load entity
				void FromJson(json& js, Cookie::ECS::EntityHandler& entity);
				//Load all components
				void FromJson(json& js, const Cookie::ECS::EntityHandler& entity, 
					Cookie::ECS::ComponentHandler& component, Cookie::Resources::ResourcesManager& resourcesManager);

				//For each functions we load all elements in the files
				void LoadScene(const char* filepath, Game& game);
				void LoadAllPrefabs(Cookie::Resources::ResourcesManager& resourcesManager);
				void LoadAllTextures(Cookie::Resources::ResourcesManager& resourcesManager);
				void LoadPhysic(json& physic, Cookie::ECS::ComponentPhysics& physics);
				void LoadGameplay(json& game, Cookie::ECS::ComponentGameplay& gameplay, Cookie::Resources::ResourcesManager& resourcesManager, bool allPrefabLoaded = false);
				void LoadVolumAndModeMusic(std::string path, Sound* const& sound);
				void LoadAllParticles(Cookie::Resources::ResourcesManager& resourcesManager);
				void LoadAllAIBehaviors(Cookie::Resources::ResourcesManager& resourcesManager);
			}
		}
	}
}


#endif // !__SERIALIZATION_HPP__
