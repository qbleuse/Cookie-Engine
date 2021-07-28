#ifndef __MINI_MAP_DRAWER_HPP__
#define __MINI_MAP_DRAWER_HPP__

#include <vector>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;

#define MINI_MAP_QUAD_COLOR 1.0f,1.0f,1.0f,1.0f

namespace Cookie
{
	namespace Resources
	{
		class Map;
		class Texture;
		class Mesh;
	}

	namespace Render
	{
		class DrawDataHandler;
		struct DrawData;
		class Camera;

		/* class that handles the drawing of the map and of the quad 
		 * representing where the camera of the player is looking */
		class MiniMapDrawer
		{
		private:

			/* shader */
			ID3D11VertexShader* VShader{ nullptr };
			ID3D11PixelShader*	PShader{ nullptr };
			ID3D11Buffer*		VCBuffer{ nullptr };
			ID3D11InputLayout*	ILayout{ nullptr };

			/* map info */
			std::unique_ptr<Resources::Mesh>	mapMesh;
			Resources::Texture*					mapAlbedo;
			Core::Math::Mat4					mapTrs;
			Core::Math::Vec2					tileNb;

			/* cam quad */
			std::unique_ptr<Resources::Mesh>	quad{ nullptr };
			std::unique_ptr<Resources::Texture> quadColor{ nullptr };
			Core::Math::Mat4					quadTrs;
		private:
			void InitShader();

		public:
			MiniMapDrawer();
			~MiniMapDrawer();

			void Set(const Camera& cam, const Resources::Map& map);
			void Draw();
		};
	}
}

#endif /* __MINI_MAP_DRAWER_HPP__ */