#ifndef __PARTICLESSYSTEM_HPP__
#define __PARTICLESSYSTEM_HPP__


#include "ParticlesEmitter.hpp"
#include "ParticlesData.hpp"
#include "ComponentTransform.hpp"
#include "RenderPass/ParticlesPass.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Camera.hpp"
#include "DrawDataHandler.hpp"
#include "Resources/ResourcesManager.hpp"

namespace Cookie
{
	namespace Resources
	{
		namespace Particles
		{
			struct emit
			{
				std::string name;
				Cookie::Core::Math::Vec3 data[2];
				std::string nameData;
			};

			struct data
			{
				Mesh* mesh;
				Texture* texture;
				int size;
				int countFrame;
				int countAlive;
				bool isBillboard;
			};

			class ParticlesPrefab
			{
			public:
				std::string name;
				std::vector<data> data;
				std::vector<ParticlesEmitter> emitter;
				std::vector<std::vector<emit>> emit;
			};

			class ParticlesSystem
			{
			public :
				//each particles systems have data (data of each particles) and a emitter who have a list of generators and updates
				std::vector<ParticlesData> data;
				std::vector<ParticlesEmitter> particlesEmiter;

				Core::Math::Mat4 trs = Core::Math::Mat4::Identity();
				bool needToBeRemoved = false;

				std::string name;

				ParticlesSystem() 
				{}

				ParticlesSystem(Cookie::Render::ParticlesPass* shader)
				{}

				~ParticlesSystem() 
				{
					data.clear();
					particlesEmiter.clear();
				}

				void generate();
				void Update();
				//For the draw we use the intancing and we don't draw if the camera don't see the particles
				void Draw(const Render::Camera& cam, Render::Frustrum& frustrum);
			};
		}
	}
}

#endif // !__PARTICLESSYSTEM_HPP__
