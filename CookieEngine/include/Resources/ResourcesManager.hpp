#ifndef __RESOURCES_MANAGER_HPP__
#define __RESOURCES_MANAGER_HPP__


#include <unordered_map>
#include <memory>
#include <filesystem>
#include <d3d11.h>
#include "Particles/ParticlesSystem.hpp"

namespace fs = std::filesystem;

namespace Cookie
{
	namespace ECS
	{
		class ComponentHandler;
		class Entity;
	}

	namespace Gameplay
	{
		struct AIBehavior;
	}

	namespace Render
	{
		class Renderer;
	}

	namespace Resources
	{

		class Mesh;
		class Shader;
		class Texture;
		class Script;
		class Prefab;
		//class Sound;

		namespace Particles
		{
			class ParticlesPrefab;
		}

		class ResourcesManager
		{
			public:
				std::unordered_map<std::string, std::unique_ptr<Mesh>>					meshes;
				std::unordered_map<std::string, std::unique_ptr<Texture>>				textures2D;
				std::unordered_map<std::string, std::unique_ptr<Texture>>				skyboxes;
				std::unordered_map<std::string, std::unique_ptr<Texture>>				icons;
				std::unordered_map<std::string, std::unique_ptr<Script>>				scripts;
				//std::unordered_map<std::string, std::unique_ptr<Sound>>					sounds;
				std::unordered_map<std::string, std::unique_ptr<Particles::ParticlesPrefab>> particles;
				std::unordered_map<std::string, std::unique_ptr<Gameplay::AIBehavior>>	aiBehaviors;
				std::unordered_map<std::string, std::unique_ptr<Prefab>>				prefabs;
				
			private:
				void SearchForAssets(const fs::path& path, std::vector<std::string>& gltfFiles);

				void InitPrimitives();
				
			public: 
				ResourcesManager();
				~ResourcesManager();

				void Load(Render::Renderer& _renderer);

				void UpdateScriptsContent();

				void CreateNewPrefabs(ECS::Entity& entity, ECS::ComponentHandler& component);
		};
	}
}

#endif /* __RESOURCES_MANAGER_HPP__ */
