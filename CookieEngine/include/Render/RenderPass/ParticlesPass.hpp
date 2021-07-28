#ifndef __PARTICLESPASS_HPP__
#define __PARTICLESPASS_HPP__

#include <reactphysics3d/reactphysics3d.h>
#include "D3D11Helper.hpp"
#include "Mat4.hpp"
#include "Vec4.hpp"


namespace Cookie
{
	namespace Render
	{
		class Camera;
	}

	namespace ECS
	{
		class Coordinator;
	}

	namespace Resources
	{
		class Mesh;
		class Texture;
	}

	namespace Render
	{
		struct InstancedData
		{
			Cookie::Core::Math::Mat4 World;
			Cookie::Core::Math::Vec4 Color;
			UINT isBillboard;
		};

		class ParticlesPass
		{
		public:
			ID3D11InputLayout* ILayout{ nullptr };
			ID3D11VertexShader* VShader{ nullptr };
			ID3D11PixelShader* PShader{ nullptr };
			ID3D11Buffer* CBuffer{ nullptr };
			ID3D11Buffer* InstanceBuffer{ nullptr };
			ID3D11BlendState* blendState{ nullptr };
			ID3D11SamplerState* PSampler{ nullptr };
			ID3D11RasterizerState* rasterizerState{ nullptr };
			ID3D11DepthStencilState* depthStencilState{ nullptr };

			std::vector<InstancedData> mInstancedData;

			void InitShader();
			void Draw(const Cookie::Render::Camera& cam, Resources::Mesh* mesh, Resources::Texture* texture, std::vector<InstancedData> data);
			void AllocateMoreSpace(int newSpace);
			void Destroy();

			void Clean();

			ParticlesPass();
			ParticlesPass(const ParticlesPass& pass);
			~ParticlesPass();
		};
	}
}

#endif