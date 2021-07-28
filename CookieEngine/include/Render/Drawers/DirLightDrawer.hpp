#ifndef __DIR_LIGHT_DRAWER_HPP__
#define __DIR_LIGHT_DRAWER_HPP__


struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11SamplerState;

namespace Cookie
{
	namespace Render
	{
		struct DirLight;
		class ShadowBuffer;

		/* draws a full screen quad that represent 
		 * the directionnal light with the shadow info */
		class DirLightDrawer
		{
		private:
			/* shader info */
			ID3D11VertexShader*	VShader{ nullptr };
			ID3D11PixelShader*	PShader{ nullptr };

			/* constant buffer containing the info of the dir light */
			ID3D11Buffer*		CBuffer{ nullptr };
			/* comparison sampler for shadows */
			ID3D11SamplerState*	CSampler{nullptr};

		private:
			void InitShader();

		public:
			DirLightDrawer();
			~DirLightDrawer();

			void Set(const DirLight& dirLight, const ShadowBuffer& shadowMap, ID3D11Buffer** lightCBuffer);
		};
	}
}

#endif /*__DIR_LIGHT_DRAWER_HPP__*/