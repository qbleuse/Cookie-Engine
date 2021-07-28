#ifndef __PRIMITIVES_HPP__
#define __PRIMITIVES_HPP__

#include "Core/Math/Vec3.hpp"
#include "Resources/Mesh.hpp"

namespace Cookie
{
	namespace Core
	{
		namespace Primitives
		{
			struct DebugVertex
			{
				Math::Vec3	vertex = {0.0f,0.0f,0.0f};
				uint32_t			color = 0;

				DebugVertex(const Math::Vec3& _vertex, uint32_t _color) :vertex{ _vertex }, color{_color} {}
			};

			inline std::unique_ptr<Resources::Mesh> CreateQuad();

			inline std::vector<DebugVertex> CreateLine(const Math::Vec3& start, const Math::Vec3& end, uint32_t color1, uint32_t color2);

			inline std::unique_ptr<Resources::Mesh> CreateTriangle();

			inline std::unique_ptr<Resources::Mesh> CreateCube();

			inline std::unique_ptr<Resources::Mesh> CreateSphere();

			inline std::unique_ptr<Resources::Mesh> CreateIcoSphere();

			inline std::unique_ptr<Resources::Mesh> CreatePyramid();

			inline std::unique_ptr<Resources::Mesh> CreateCylinder();

			inline std::unique_ptr<Resources::Mesh> CreateCone();

			inline std::unique_ptr<Resources::Mesh> CreateCircle();

			inline std::unique_ptr<Resources::Mesh> CreateCapsule();
		}
	}
}


#include "Primitives.inl"
#include "Sphere.inl"

#endif // !__PRIMITIVES_HPP__
