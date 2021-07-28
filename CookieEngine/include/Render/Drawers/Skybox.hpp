#ifndef __SKYBOX_HPP__
#define __SKYBOX_HPP__

struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;

#include <memory>

namespace Cookie
{
	namespace Resources
	{
		class ResourcesManager;
		class Mesh;
		class Texture;
	}

	namespace Core::Math
	{
		union Mat4;
	}

	namespace Render
	{
		class SkyBox
		{
			private:
				/* Matrix Constant buffer */
				ID3D11Buffer*		CBuffer{ nullptr };

				ID3D11InputLayout*	ILayout{ nullptr };
				ID3D11VertexShader* VShader{ nullptr };
				ID3D11PixelShader*  PShader{ nullptr };
				ID3D11SamplerState* PSampler{ nullptr };


			public:
				/* model Info */
				std::unique_ptr<Cookie::Resources::Mesh>	cube			{ nullptr };
				Cookie::Resources::Texture*					texture			{ nullptr };

				ID3D11RasterizerState*		rasterizerState	{ nullptr };


			private:
				void InitShader();
				void InitRasterizer();

			public:

				/* CONSTRUCTORS/DESTRUCTORS */
				SkyBox();
				~SkyBox();

				void Draw(const Core::Math::Mat4& proj, const Core::Math::Mat4& view);

		};
	}
}


#endif // !__SKYBOX_HPP__