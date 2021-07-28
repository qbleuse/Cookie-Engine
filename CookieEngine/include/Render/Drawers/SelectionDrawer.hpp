#ifndef __SELECTION_DRAWER_HPP__
#define __SELECTION_DRAWER_HPP__

#include <Mat4.hpp>

struct ID3D11Buffer;
struct ID3D11DepthStencilView;

#define ARROW_COLOR {0.0f,1.0f,0.0f,0.3f}
#define OUTLINE_COLOR {1.0f,1.0f,1.0f,0.5f}
#define OUTLINE_SIZE {1.21f,1.21f,1.21f}

namespace Cookie
{
	namespace Resources
	{
		class Texture;
		class Mesh;
	}

	namespace Render
	{
		struct DrawData;
		class DrawDataHandler;

		/* class to help draws the outline of selected models and arrows of building */
		class SelectionDrawer
		{
			private:
				Core::Math::Mat4 outLineSize = Core::Math::Mat4::Scale(OUTLINE_SIZE);
				std::unique_ptr<Resources::Mesh> arrowMesh{ nullptr };
				ID3D11DepthStencilView* stencilBuffer;

			public:
				Core::Math::Mat4 viewProj;

				const std::vector<DrawData>*	selectedDrawData;

				std::vector<Core::Math::Mat4> arrowsMatrices;
			public:
				SelectionDrawer();
				~SelectionDrawer();

				void Set(const DrawDataHandler& drawData);
				void FillStencil(ID3D11Buffer* VCBuffer);
				void Draw(ID3D11Buffer* VCBuffer, ID3D11Buffer* PCBuffer);
				void Clear();
		};
	}
}

#endif // !__SELECTION_DRAWER_HPP__
