#ifndef __FRAME_BUFFER_HPP__
#define __FRAME_BUFFER_HPP__

#include <dxgiformat.h>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;

namespace Cookie
{
	namespace Render
	{
		class FrameBuffer
		{
			public:
				ID3D11Texture2D*			texBuffer			{ nullptr };
				ID3D11ShaderResourceView*	shaderResource		{ nullptr };
				ID3D11RenderTargetView*		renderTargetView	{ nullptr };
				DXGI_FORMAT					format				{ DXGI_FORMAT_UNKNOWN };
				int							width				= 0;
				int							height				= 0;


			private:
				bool CreateTexture();
				bool CreateShaderResource();
				bool CreateRenderTargetView();

			public:
				/* CONSTRUCTORS/DESTRUCTORS */
				FrameBuffer(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_UNORM);
				~FrameBuffer();

				void Resize(int width, int height);
		};
	}
}

#endif // !__FRAME_BUFFER_HPP__
