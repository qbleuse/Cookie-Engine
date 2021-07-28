#ifndef __POINT_LIGHT_DRAWER_HPP__
#define __POINT_LIGHT_DRAWER_HPP__

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;

namespace Cookie
{
	namespace Resources
	{
		class Mesh;
	}

	namespace Render
	{
		struct PointLight;
		class DrawDataHandler;

		/* class  that draws sphere that represent 
		 * point lights with instanced info */
		class PointLightDrawer
		{
		private:
			/* shader info */
			ID3D11VertexShader* VShader{ nullptr };
			ID3D11PixelShader*	PShader{ nullptr };
			ID3D11InputLayout*	ILayout{ nullptr };

			/* Instance buffer */
			ID3D11Buffer*		IBuffer	{ nullptr };

			std::unique_ptr<Resources::Mesh> sphereMesh{ nullptr };

		private:
			void InitShader();

		public:
			PointLightDrawer();
			~PointLightDrawer();

			void Set(const LightsArray& lights, const DrawDataHandler& drawData);
			void FillStencil(const unsigned int instanceNb);
			void Draw(ID3D11Buffer** lightCBuffer, const unsigned int instanceNb);
		};
	}
}

#endif /*__POINT_LIGHT_DRAWER_HPP__*/