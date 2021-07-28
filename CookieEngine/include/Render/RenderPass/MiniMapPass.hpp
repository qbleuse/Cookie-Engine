#ifndef __MINI_MAP_PASS_HPP__
#define __MINI_MAP_PASS_HPP__

struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;

#include <memory>
#include "Drawers/MiniMapDrawer.hpp"
#include "Drawers/MiniModelDrawer.hpp"

struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;

namespace Cookie
{
	namespace Resources
	{
		class Texture;
		class Mesh;
	}

	namespace Render
	{


		class FrameBuffer;

		class DrawDataHandler;

		/* class that draws the game in orthographic projection */
		class MiniMapPass
		{
		private:
			D3D11_VIEWPORT viewport = {};
			
		public:
			/* camera matrices of the mini Map*/
			Core::Math::Mat4 ortho;
			Core::Math::Mat4 miniMapView;

			/* darws the map */
			MiniMapDrawer		miniMapDrawer;

			/* draws the model on the map */
			MiniModelDrawer		miniModelDrawer;

			/* dx11 state */
			ID3D11DepthStencilState*	depthStencilState;
			ID3D11RasterizerState*		rasterState;

			/* depth Buffer */
			ID3D11DepthStencilView* depthBuffer = nullptr;
		private:
			void InitState();

		public:
			void CreateDepth(int width, int height);

			MiniMapPass();
			~MiniMapPass();

			void Draw(DrawDataHandler& drawData, const FrameBuffer& frameBuffer);
		};
	}
}

#endif /*__MINI_MAP_PASS_HPP__*/