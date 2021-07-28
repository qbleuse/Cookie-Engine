#ifndef __MAP_DRAWER_HPP__
#define __MAP_DRAWER_HPP__

#include <Mat4.hpp>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;

#define MAP_TILE_LIMITS {0.01f,0.99f}

namespace Cookie
{
	namespace Resources
	{
		class Texture;
		class Mesh;
		class Map;
	}

	namespace Render
	{

		/* class that handles the drawing of the Map */
		class MapDrawer
		{
			private:
				/* the dx11 info to display the map */
				ID3D11VertexShader* VShader	{ nullptr };
				ID3D11Buffer*		VCBuffer{ nullptr };
				ID3D11InputLayout*	ILayout	{ nullptr };

			public:
				/* map model Info */
				std::unique_ptr<Resources::Mesh>	mapMesh;
				Resources::Texture*					albedoTex = nullptr;
				Resources::Texture*					normalTex = nullptr;
				Resources::Texture*					matTex = nullptr;

				/* special info */
				Core::Math::Mat4					model;
				Core::Math::Vec2					tileNb;


			private:
				void InitShader();

			public:
				MapDrawer();
				~MapDrawer();

				void Set(const Resources::Map& map);
				void Draw();
		};
	}
}

#endif // !__MAP_DRAWER_HPP__
