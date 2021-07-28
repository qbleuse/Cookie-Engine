#ifndef __LOADER_HPP__
#define __LOADER_HPP__

#include <assimp/Importer.hpp>
#include <assimp/material.h>

namespace Cookie
{
	namespace Resources
	{
		class ResourcesManager;

		class Loader
		{
			private:
				Assimp::Importer importer;

			public:
				/* CONSTRUCTORS/DESTRUCTORS */
				Loader();
				~Loader();

				void Load(const char* fileName, ResourcesManager& _resources, Render::Renderer& _renderer);
				void InitScene(const char* fileName, const aiScene* _scene, ResourcesManager& _resources, Render::Renderer& _renderer);
				void InitMeshes(const char* fileName, aiMesh** meshes, unsigned int nMeshes, ResourcesManager& _resources, Render::Renderer& _renderer);
				void InitTextures(const char* pathToFile, aiMaterial** materials, unsigned int nMatNb, ResourcesManager& _resources, Render::Renderer& _renderer);
		};
	}
}

#endif /* __LOADER_HPP__ */
