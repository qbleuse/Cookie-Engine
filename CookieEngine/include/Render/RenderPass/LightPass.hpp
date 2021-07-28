#ifndef __LIGHT_PASS_HPP__
#define __LIGHT_PASS_HPP__

#include "FrameBuffer.hpp"
#include "Drawers/DirLightDrawer.hpp"
#include "Drawers/PointLightDrawer.hpp"

namespace Cookie
{
	namespace Render
	{
		struct	LightsArray;
		class	ShadowBuffer;
		class	DrawDataHandler;

		/* class that represent the third pass of the derrered renderer, compute the lights
		 * only directionnal and 64 point lights are supported */
		class LightPass
		{
			private:
				DirLightDrawer				dirDrawer;
				PointLightDrawer			pointDrawer;

				/* for shader */
				ID3D11Buffer*				lightCBuffer		{ nullptr };
				ID3D11SamplerState*			PSampler			{ nullptr };

				/* dx11 state */
				ID3D11DepthStencilState*	depthStencilState	= nullptr;
				ID3D11RasterizerState*		rasterizerState		= nullptr;
				ID3D11BlendState*			blendState			= nullptr;
				/* special state for point light support */
				ID3D11RasterizerState*		volumeRasterState	= nullptr;
				ID3D11DepthStencilState*	volumeStencilState	= nullptr;

			public:
				FrameBuffer diffuseFBO;
				FrameBuffer specularFBO;
				
			private:
				void InitShader();
				void InitState();

			public:
				LightPass(int width, int height);
				~LightPass();

				void Set(FrameBuffer& posFBO, FrameBuffer& normalFBO, FrameBuffer& albedoFBO);
				void Draw(const ShadowBuffer& shadowMap, const DrawDataHandler& drawData);
				void Clear();
		};
	}
}

#endif /*__LIGHT_PASS_HPP__*/