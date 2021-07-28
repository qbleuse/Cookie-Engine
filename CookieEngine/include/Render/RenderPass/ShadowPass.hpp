#ifndef __SHADOW_PASS_HPP__
#define __SHADOW_PASS_HPP__

struct ID3D11VertexShader;
struct ID3D11Buffer;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct D3D11_VIEWPORT;

#include <vector>
#include <ShadowBuffer.hpp>

namespace Cookie
{
	namespace Core::Math
	{
		union Vec4;
		union Mat4;
	}

	namespace ECS
	{
		class Coordinator;
	}

	namespace Resources
	{
		class Map;
	}

	namespace Render
	{
		struct LightsArray;
		class DrawDataHandler;
		class Camera;

		/* class representing a render pass where we fill a shadowmap */
		class ShadowPass
		{

		private:
			/* shader */
			ID3D11VertexShader*			VShader{ nullptr };
			ID3D11Buffer*				CBuffer{ nullptr };

			/* dx11 state */
			ID3D11DepthStencilState*	depthStencilState	= nullptr;
			ID3D11RasterizerState*		rasterizerState		= nullptr;

			/* viewport */
			D3D11_VIEWPORT				shadowViewport;
			Core::Math::Mat4			proj;
			
		public:
			/* the shadowmap (depthStencilView and ShaderResourcesView)*/
			ShadowBuffer				shadowMap;

		private:
			void InitShader();
			void InitState();

		public:
			ShadowPass();
			~ShadowPass();

			void Set();
			void Draw(DrawDataHandler& drawData);
			void Clear();
		};
	}
}

#endif /*__SHADOW_PASS_HPP__*/