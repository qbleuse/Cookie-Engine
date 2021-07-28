#ifndef __PLAYER_DRAWER_HPP__
#define __PLAYER_DRAWER_HPP__

#include <Mat4.hpp>

struct ID3D11Buffer;

#define PLAYER_INIT_VALID_COLOR {0.0f,1.0f,0.0f,0.5f}
#define PLAYER_INIT_INVALID_COLOR {1.0f,0.0f,0.0,0.5f}

namespace Cookie
{
	namespace Resources
	{
		class Texture;
		class Mesh;
	}

	namespace Render
	{
		class DrawDataHandler;

		/* class to help draws the selection quad and the building in construction */
		class PlayerDrawer
		{
		private:
			std::unique_ptr<Resources::Mesh> quadMesh { nullptr };

		public:
				Core::Math::Mat4 viewProj;

				Core::Math::Vec4 validColor		PLAYER_INIT_VALID_COLOR ;
				Core::Math::Vec4 invalidColor	PLAYER_INIT_INVALID_COLOR ;
				bool isValid					= false;
				
				/* for the selection quad, vertex buffer will be changed each time */
				bool isMakingQuad				= false;
				Core::Math::Mat4				quadTrs;

				/* if player is making a building, buildingMesh will not be at nullptr
				 * so it also acts as boolean */
				Resources::Mesh*	buildingMesh		= nullptr;
				Resources::Texture* buildingAlbedoTex	= nullptr;
				Core::Math::Mat4	buildingTRS;

		public:
			PlayerDrawer();
			~PlayerDrawer();

			void Set(const DrawDataHandler& drawData);
			void Draw(ID3D11Buffer* VCBuffer, ID3D11Buffer* PCBuffer);
		};
	}
}

#endif // !__PLAYER_DRAWER_HPP__
