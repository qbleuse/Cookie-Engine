#ifndef __GEOMETRY_PASS_HPP__
#define __GEOMETRY_PASS_HPP__

#include "FrameBuffer.hpp"

namespace Cookie
{

	namespace Render
	{
		class DrawDataHandler;
		class Camera;

		/* class that represent the first pass of a deffered renderer:
		 * it puts into framebuffers the position, normals and albedo (or color)
		 * of the models. */
		class GeometryPass
		{
			private:
				/* Shader Info */
				ID3D11InputLayout*	ILayout{ nullptr };
				ID3D11VertexShader* VShader{ nullptr };
				ID3D11PixelShader*	PShader{ nullptr };
				ID3D11SamplerState*	PSampler{ nullptr };

				/* dx11 State Info */
				ID3D11DepthStencilState*	depthStencilState	= nullptr;
				ID3D11RasterizerState*		rasterizerState		= nullptr;
				ID3D11BlendState*			blendState			= nullptr;

			public:
				/* depth buffer and camera framebuffer */
				ID3D11DepthStencilView* depthBuffer = nullptr;
				ID3D11Buffer* CBuffer{ nullptr };

				FrameBuffer	posFBO;
				FrameBuffer	normalFBO;
				FrameBuffer	albedoFBO;

			private:
				void InitShader();
				void InitState();

			public:
				void CreateDepth(int width, int height);

				GeometryPass(int width, int height);
				~GeometryPass();

				void Set();
				void Draw(DrawDataHandler& drawData);
				void Clear();
		};
	}
}

#endif /*__GEOMETRY_PASS_HPP__*/