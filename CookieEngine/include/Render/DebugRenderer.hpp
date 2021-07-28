#ifndef __DEBUG_RENDERER_HPP__
#define __DEBUG_RENDERER_HPP__

#include <reactphysics3d/reactphysics3d.h>
#include "D3D11Helper.hpp"

namespace Cookie
{
	namespace Core
	{
		namespace Math
		{
			union Mat4;
		}

		namespace Primitives
		{
			struct DebugVertex;
		}
	}

	namespace Render
	{
		/* class that draws the primitives given by reactphysics3D and can draw line */
		class DebugRenderer
		{
			private:
				/* shader */
				ID3D11Buffer*		VBuffer	{ nullptr };
				ID3D11VertexShader*	VShader	{ nullptr };
				ID3D11PixelShader*	PShader	{ nullptr };
				ID3D11Buffer*		CBuffer	{ nullptr };
				ID3D11InputLayout*	ILayout	{ nullptr };


				D3D11_BUFFER_DESC			bDesc		{};
				ID3D11RasterizerState*		rasterState	{ nullptr };

				rp3d::DebugRenderer&								physDbgRenderer;
				std::vector<Core::Primitives::DebugVertex>			debugElement;

				rp3d::DebugRenderer& InitEdit();
				void InitRasterizerState();
				void InitShader();
				void AllocateVBuffer(size_t vBufferSize);
				void UpdateVBuffer(size_t vBufferSize, void* data);

			public:
				bool showDebug = true;

				void Draw(const Core::Math::Mat4& viewProj);

				/* adds lines in debug element */
				void AddDebugElement(const std::vector<Core::Primitives::DebugVertex>& dbgElement);
				void AddLine(Core::Math::Vec3 a, Core::Math::Vec3 b, uint32_t color);
				void AddQuad(Core::Math::Vec3 start, Core::Math::Vec3 end, uint32_t color);
				void AddQuad(Core::Math::Vec3 center, float halfWidth, float halfDepth, uint32_t color);


				inline void Reset()noexcept { physDbgRenderer.reset(); };

				DebugRenderer();
				~DebugRenderer();

		};
	}
}

#endif // !__DEBUG_RENDERER_HPP__