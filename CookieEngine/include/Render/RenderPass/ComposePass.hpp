#ifndef __COMPOSE_PASS_HPP__
#define __COMPOSE_PASS_HPP__

namespace Cookie
{
	namespace Render
	{
		class FrameBuffer;

		/* class that represent the last pass of the deferred renderer.
		 * it blend the lights and the albedo and does a tone mapping */
		class ComposePass
		{
			private:
				/* shader */
				ID3D11VertexShader*			VShader				{ nullptr };
				ID3D11PixelShader*			PShader				{ nullptr };
				ID3D11SamplerState*			PSampler			{ nullptr };

				/* dx11 state */
				ID3D11DepthStencilState*	depthStencilState	= nullptr;
				ID3D11RasterizerState*		rasterizerState		= nullptr;
				ID3D11BlendState*			blendState			= nullptr;
			private:
				void InitShader();
				void InitState();

			public:
				ComposePass();
				~ComposePass();

				void Set(FrameBuffer& diffuse, FrameBuffer& specular, FrameBuffer& albedo);
				void Draw();
		};
	}
}

#endif // !__COMPOSE_PASS_HPP__
