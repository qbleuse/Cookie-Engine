#ifndef __GAMEPLAY_PASS_HPP__
#define __GAMEPLAY_PASS_HPP__

#include "Render/Drawers/PlayerDrawer.hpp"
#include "Render/Drawers/SelectionDrawer.hpp"

struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;

namespace Cookie
{
	namespace Render
	{
		class DrawDataHandler;

		/* class that represent Post Process pass that draws gameplay based info */
		class GameplayPass
		{
			private:
				/* shader */
				ID3D11VertexShader*			VShader{ nullptr };
				ID3D11PixelShader*			PShader{ nullptr };
				ID3D11Buffer*				VCBuffer{ nullptr };
				ID3D11Buffer*				PCBuffer{ nullptr };

				/* dx11 state */
				ID3D11RasterizerState*		rasterizerState		{ nullptr };
				ID3D11DepthStencilState*	depthStencilState	{ nullptr };
				ID3D11DepthStencilState*	outLineState		{ nullptr };
				ID3D11BlendState*			blendState			{ nullptr };
				ID3D11SamplerState*			PSampler			{ nullptr };

			public:
				/* draws the plaver's selection quad and 
				 * the building when you want to build one */
				PlayerDrawer				playerDrawer;
				/* draw the selected entities and the arrow */
				SelectionDrawer				selectDrawer;

			private:

				void InitShader();
				void InitState();

			public:
				GameplayPass();
				~GameplayPass();

				void Set();
				void Draw(const DrawDataHandler& drawData);
		};
	}
}

#endif /*__GAMEPLAY_PASS_HPP__*/