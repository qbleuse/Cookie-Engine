#include <d3d11.h>
#include <algorithm>

#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Resources/Script.hpp"
#include "Resources/Scene.hpp"
#include "Resources/Prefab.hpp"
//#include "Resources/Sound.hpp"
#include "Resources/Loader.hpp"
#include "Resources/ResourcesManager.hpp"

#include "Render/Renderer.hpp"
#include "Core/Primitives.hpp"
#include "ECS/EntityHandler.hpp"

#include <memory>

using namespace Cookie::Resources;

ResourcesManager::ResourcesManager()
{
	//scripts["test.lua"] = std::make_shared<Script>("Assets\\scripts\\test.lua");
	//scripts["test2.lua"] = std::make_shared<Script>("Assets\\scripts\\test2.lua");

	InitPrimitives();
}

ResourcesManager::~ResourcesManager()
{
}


void ResourcesManager::SearchForAssets(const fs::path& path, std::vector<std::string>& assetsFiles)
{
	if (fs::exists(path) && fs::is_directory(path))
	{
		for (const fs::directory_entry& entry : fs::directory_iterator(path))
		{
			const fs::path& filename = entry.path().c_str();
			if (fs::is_directory(entry.status()))
			{
				SearchForAssets(filename, assetsFiles);
			}
		}

		for (const fs::directory_entry& entry : fs::directory_iterator(path))
		{
			const fs::path& filepath = entry.path().c_str();
			if (fs::is_regular_file(entry.status()))
			{
				if		(filepath.string().find(".gltf") != std::string::npos)
					assetsFiles.push_back(filepath.string());
				else if (filepath.string().find(".png") != std::string::npos)
					assetsFiles.push_back(filepath.string());
				else if (filepath.string().find(".dds") != std::string::npos)
					assetsFiles.push_back(filepath.string());
				else if (filepath.string().find(".jpg") != std::string::npos)
					assetsFiles.push_back(filepath.string());
				else if (filepath.string().find(".ico") != std::string::npos)
					assetsFiles.push_back(filepath.string());
				else if (filepath.string().find(".tif") != std::string::npos)
					assetsFiles.push_back(filepath.string());
			}

		}
	}
}

void ResourcesManager::InitPrimitives()
{
	meshes["Quad"]		= Cookie::Core::Primitives::CreateQuad();
	meshes["Triangle"]	= Cookie::Core::Primitives::CreateTriangle();
	meshes["Cube"]		= Cookie::Core::Primitives::CreateCube();
	meshes["Sphere"]	= Cookie::Core::Primitives::CreateSphere();
	meshes["Pyramid"]	= Cookie::Core::Primitives::CreatePyramid();
	meshes["IcoSphere"]	= Cookie::Core::Primitives::CreateIcoSphere();
	meshes["Cylinder"]	= Cookie::Core::Primitives::CreateCylinder();
	meshes["Cone"]		= Cookie::Core::Primitives::CreateCone();
	meshes["Circle"]	= Cookie::Core::Primitives::CreateCircle();
	meshes["Capsule"]	= Cookie::Core::Primitives::CreateCapsule();
}

void ResourcesManager::Load(Render::Renderer& _renderer)
{
	std::vector<std::string> assetsFiles;
	SearchForAssets(std::string("Assets/"), assetsFiles);

	Loader loader;

	for (unsigned int i = 0; i < assetsFiles.size(); i++)
	{
		std::string& iFile = assetsFiles.at(i);
		std::replace(iFile.begin(),iFile.end(),'\\','/');
		//printf("%s\n", gltfFiles.at(i).c_str());
	}
	
	/* load models */
	for (unsigned int i = 0; i < assetsFiles.size(); i++)
	{
		std::string& iFile = assetsFiles.at(i);
		//printf("%s\n", iFile.c_str());
		if (iFile.find(".gltf") != std::string::npos)
		{
			loader.Load(iFile.c_str(), *this, _renderer);
			assetsFiles.erase(assetsFiles.begin() + i);
			i--;
		}
		//printf("%s\n", gltfFiles.at(i).c_str());
	}

	for (unsigned int i = 0; i < assetsFiles.size(); i++)
	{
		std::string& iFile = assetsFiles.at(i);


		if (textures2D.find(iFile) == textures2D.end() && icons.find(iFile) == icons.end() && skyboxes.find(iFile) == skyboxes.end())
		{
			std::unique_ptr<Texture> aTexture = std::make_unique<Texture>(iFile);
			if (iFile.find("Assets/EditorUIcons/") != std::string::npos || iFile.find("Assets/UI/") != std::string::npos)
			{
				icons[iFile] = std::move(aTexture);
			}
			else
			{
				if (aTexture->desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
				{
					textures2D[iFile] = std::move(aTexture);
				}
				else if (aTexture->desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURECUBE)
				{
					skyboxes[iFile] = std::move(aTexture);
				}
			}
		}
		//printf("%s\n", iFile.c_str());
	}
}

void ResourcesManager::UpdateScriptsContent()
{
	for (std::unordered_map<std::string, std::unique_ptr<Script>>::iterator scrIt = scripts.begin(); scrIt != scripts.end(); scrIt++)
		if (!(scrIt->second->isUpToDate()))
			scrIt->second->UpdateContent();
}

void ResourcesManager::CreateNewPrefabs(ECS::Entity& entity, ECS::ComponentHandler& component)
{
	Prefab newPrefab;

	newPrefab.name = entity.name;
	newPrefab.signature = entity.signature;
	newPrefab.filepath = "Assets/Prefabs/" + entity.name + ".PAsset";
	if (entity.signature & Cookie::ECS::C_SIGNATURE::TRANSFORM)
		newPrefab.transform = component.GetComponentTransform(entity.id);
	if (entity.signature & Cookie::ECS::C_SIGNATURE::MODEL)
		newPrefab.model = component.GetComponentModel(entity.id);
	if (entity.signature & Cookie::ECS::C_SIGNATURE::PHYSICS)
		newPrefab.physics = component.GetComponentPhysics(entity.id);
	if (entity.signature & Cookie::ECS::C_SIGNATURE::GAMEPLAY)
		newPrefab.gameplay = component.GetComponentGameplay(entity.id);

	entity.namePrefab = entity.name;

	prefabs[newPrefab.name] = std::make_unique<Prefab>(newPrefab);
}




