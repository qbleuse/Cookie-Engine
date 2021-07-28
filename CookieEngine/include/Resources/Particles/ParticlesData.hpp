#ifndef __PARTICLESDATA_HPP__
#define __PARTICLESDATA_HPP__

#include <vector>
#include "Vec4.hpp"
#include "ParticlesEmitter.hpp"
#include "ParticlesGenerator.hpp"
#include "Mat4.hpp"

namespace Cookie
{
	namespace Core::Math
	{
		union Vec3;
	}
	namespace Resources
	{
		class Mesh;
		class Shader;
		class Texture;

		namespace Particles
		{  
			struct Data
			{
				Cookie::Core::Math::Vec4 col = Cookie::Core::Math::Vec4(1, 1, 1, 1);
				Cookie::Core::Math::Vec4 colBegin = Cookie::Core::Math::Vec4(1, 1, 1, 1);
				Cookie::Core::Math::Vec3 pos = Cookie::Core::Math::Vec3(0, 0, 0);
				Cookie::Core::Math::Vec3 scale = Cookie::Core::Math::Vec3(1, 1, 1);
				Cookie::Core::Math::Vec3 scaleBegin = Cookie::Core::Math::Vec3(1, 1, 1);
				Cookie::Core::Math::Vec3 vel = Cookie::Core::Math::Vec3(0, 0, 0);
				Cookie::Core::Math::Vec3 rot = Cookie::Core::Math::Vec3(0, 0, 0);
				float					 time = 1;
				float					 timeMax = 1;
				float					 mass = 1.0f;
				bool					 isBillboard = true;
				bool					 alive = false;
			};

			class ParticlesData
			{
			public:
				//Data we modify with generators and updators
				std::vector<Data> data;

				Cookie::Resources::Mesh*				mesh = nullptr;
				Cookie::Resources::Texture*				texture = nullptr;
				int										countFrame = 0;
				int										countAlive = 0;
				bool									canRemoved = true;

				ParticlesData() {}
				~ParticlesData() {}

				void generate(int size, int sizeFrame) 
				{
					countFrame = sizeFrame;
					countAlive = 0;
					data.resize(size);
				}

				void SetIsBIllboard(bool value)
				{
					for (int i = 0; i < data.size(); i++)
						data[i].isBillboard = value;
				}

				void kill(int index) 
				{
					data[index].alive = false;
					swapData(index, countAlive - 1);
					countAlive -= 1;
				}

				void wake(int indexBegin, int indexEnd)
				{
					for (int i = indexBegin; i < indexEnd; i++)
					{
						data[i].alive = true;
						countAlive += 1;
						data[i].pos = Cookie::Core::Math::Vec3(0, 0, 0);
						data[i].scale = Cookie::Core::Math::Vec3(1, 1, 1);
						data[i].rot = Cookie::Core::Math::Vec3(0, 0, 0);
						data[i].time = 10;
						data[i].timeMax = 10;
					}
				}

				void swapData(int indexA, int indexB) 
				{
					std::iter_swap(data.begin() + indexA, data.begin() + indexB);
				}
			};
			
		}
	}
}


#endif // !__PARTICLESDATA_HPP__
