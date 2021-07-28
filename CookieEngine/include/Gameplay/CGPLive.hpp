#ifndef _CGP_LIVE_HPP__
#define _CGP_LIVE_HPP__

#include "Vec3.hpp"

namespace Cookie
{
	namespace Resources
	{
		class Sound;

		namespace Particles
		{
			class ParticlesPrefab;
		}
	}

	namespace ECS
	{
		class ComponentTransform;
	}

	namespace Gameplay
	{
		class CGPLive
		{
		public:

			ECS::ComponentTransform*               trs      {nullptr};
			Resources::Sound*					   sfxDeath {nullptr};
			Resources::Particles::ParticlesPrefab* vfxDeath {nullptr};

			Resources::Sound*					   sfxHit   {nullptr};
			Resources::Particles::ParticlesPrefab* vfxHit   {nullptr};


			float lifeMax = 10;
			float lifeCurrent = 10;
			float armor = 0;
			Cookie::Core::Math::Vec3 lifeBarOffset = Cookie::Core::Math::Vec3(0, 0, 0);

			CGPLive() {}
			~CGPLive() {}

			inline void ToDefault() noexcept
			{
				sfxDeath = nullptr;
				vfxDeath = nullptr;

				sfxHit = nullptr;
				vfxHit = nullptr;

				lifeMax = 10;
				lifeCurrent = 10;
				armor = 0;
				lifeBarOffset = Cookie::Core::Math::Vec3(0, 0, 0);
			}

			void TakeHit(const float damage);
		};


	}
}

#endif // !_CGP_LIVE_HPP__