#ifndef __SHADOW_BUFFER_HPP__
#define __SHADOW_BUFFER_HPP__

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

#define SHADOW_MAP_RESOLUTION 4096

namespace Cookie
{
	namespace Render
	{
		/* class that is like the framebuffer class but for shadow. 
		 * it creates a shadowMap with SHADOW_MAP_RESOLUTION */
		class ShadowBuffer
		{
		public:
			ID3D11Texture2D*			texBuffer{ nullptr };
			ID3D11ShaderResourceView*	shaderResource{ nullptr };
			ID3D11DepthStencilView*		depthStencilView{ nullptr };

		private:
			bool CreateTexture();
			bool CreateShaderResource();
			bool CreateDepthStencilView();

		public:
			/* CONSTRUCTORS/DESTRUCTORS */
			ShadowBuffer();
			~ShadowBuffer();

			void Clear();
		};
	}
}

#endif // !__SHADOW_BUFFER_HPP__
