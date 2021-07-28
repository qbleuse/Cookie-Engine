#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <d3d11.h>
#include <string>


namespace Cookie
{
	namespace Core::Math
	{
		union Vec4;
	}

	namespace Render
	{
		class Renderer;
		struct RendererRemote;
	}

	namespace Resources
	{
		class Texture
		{
			private:
				ID3D11Resource*				texture				= nullptr;
				ID3D11ShaderResourceView*	shaderResourceView	= nullptr;

			public:
				std::string name;
				D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};

			private:
				bool CreateTextureFromColor(const Core::Math::Vec4& color);
				bool CreateShaderResource(DXGI_FORMAT format);

			public:
				/* CONSTRUCTORS/DESTRUCTORS */
				Texture(const std::string& texPath);
				Texture(const std::string& texName, const Core::Math::Vec4& color);
				~Texture();


				void Set(unsigned int slot = 0u);

				inline ID3D11ShaderResourceView* GetResourceView() const { return shaderResourceView; };
		};
	}
}

#endif // !__TEXTURE_HPP__
