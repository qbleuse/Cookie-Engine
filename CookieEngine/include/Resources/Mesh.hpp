#ifndef __MESH_HPP__
#define __MESH_HPP__

struct ID3D11Buffer;

#include "Core/Math/Vec3.hpp"

#include <string>
#include <assimp/mesh.h>
#include <vector>

namespace Cookie
{
	namespace Render
	{
		class Renderer;
		struct RendererRemote;
	}

	namespace Resources
	{
		/* the mesh are normalized during import to be all in a box between 
		 * -0.5 and 0.5 on all axes */
		class Mesh
		{
			public:
				ID3D11Buffer* VBuffer	= nullptr;
				ID3D11Buffer* IBuffer	= nullptr;
				unsigned int  INb		= 0;

				/* cannot exceed 0.5 or -0.5 but can be smaller */
				Core::Math::Vec3 AABBMin;
				Core::Math::Vec3 AABBMax;

				std::string name;
				

			private:
				void InitVBuffer(aiMesh* mesh);
				void InitIBuffer(aiMesh* mesh);
				void InitVBuffer(std::vector<float>& vertices);
				void InitIBuffer(std::vector<unsigned int>& indices);

				void ComputeAABB(const std::vector<float>& vertices);

			public:
				/* CONSTRUCTORS/DESTRUCTORS */
				Mesh(std::string meshName, aiMesh* mesh);
				Mesh(std::string meshName, std::vector<float>& vertices, std::vector<unsigned int>& indices, unsigned int INb);
				~Mesh();

				void Set()const;
				void Draw()const;
		};
	}
}

#endif /* __MESH_HPP__ */