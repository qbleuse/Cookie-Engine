#ifndef __PARTICLESUPADATE_HPP__
#define __PARTICLESUPADATE_HPP__

#include "Vec3.hpp"
#include "Vec4.hpp"
#include <vector>

namespace Cookie
{
	namespace Resources
	{
		class ResourcesManager;

		namespace Particles
		{
			class ParticlesData;
			class ParticlesGenerator;
			class ParticlesHandler;

			enum TYPEUP
			{
				UPDATEVEL,
				UPDATESCALE,
				UPDATEALPHA,
				COLOROVERLIFE,
				ENABLEGRAVITY,
				UPDATETIME,
				LOOP,
				COLLISIONWITHPLANE,
				CREATEPARTICLES,
				SHADOW,
				SPAWNEND,
				STOPAT
			};

			std::string GetName(const TYPEUP& type);

			class ParticlesUpdate
			{
			public :
				virtual void Update(ParticlesData* p) = 0;
				TYPEUP type;

				ParticlesUpdate() {}
				virtual ~ParticlesUpdate() {}
			};

			class UpdateVelocity : public ParticlesUpdate
			{
			public :
				UpdateVelocity() { type = TYPEUP::UPDATEVEL; }
				~UpdateVelocity() {}

				virtual void Update(ParticlesData* p) override;
			};


			class UpdateScale : public ParticlesUpdate
			{
			public:
				Cookie::Core::Math::Vec3 scaleEnd = Cookie::Core::Math::Vec3(0, 0, 0);
				UpdateScale(Cookie::Core::Math::Vec3 scale = Cookie::Core::Math::Vec3(0, 0, 0)) : scaleEnd(scale) { type = TYPEUP::UPDATESCALE; }
				~UpdateScale()override {}

				virtual void Update(ParticlesData* p) override;
			};

			class UpdateAlpha : public ParticlesUpdate
			{
			public:
				float alphaEnd = 0;
				UpdateAlpha(float alpha = 0): alphaEnd(alpha) { type = TYPEUP::UPDATEALPHA; }
				~UpdateAlpha()override {}

				virtual void Update(ParticlesData* p) override;
			};

			class ColorOverLife : public ParticlesUpdate
			{
			public:
				Cookie::Core::Math::Vec4 colorEnd = Cookie::Core::Math::Vec4(0, 0, 0, 0);
				ColorOverLife(Cookie::Core::Math::Vec4 color = Cookie::Core::Math::Vec4(1, 1, 1, 1)) : colorEnd(color) { type = TYPEUP::COLOROVERLIFE; }
				~ColorOverLife()override {}

				virtual void Update(ParticlesData* p) override;
			};

			class EnabledGravity : public ParticlesUpdate
			{
			public:
				float gravity = -9.81f;
				EnabledGravity(float gravity = -9.81f) : gravity(gravity) { type = TYPEUP::ENABLEGRAVITY; }
				~EnabledGravity()override {}

				virtual void Update(ParticlesData* p) override;
			};

			class UpdateTime : public ParticlesUpdate
			{
			public :
				UpdateTime() { type = TYPEUP::UPDATETIME; }
				~UpdateTime()override {}

				virtual void Update(ParticlesData* p) override;
			};

			class Loop : public ParticlesUpdate
			{
			public:
				Loop() { type = TYPEUP::LOOP; }
				Loop(std::vector<std::shared_ptr<ParticlesGenerator>>& _particlesGen) : particlesGen(&_particlesGen) { type = TYPEUP::LOOP; }
				~Loop()override {}
				std::vector<std::shared_ptr<ParticlesGenerator>>* particlesGen;

				virtual void Update(ParticlesData* p) override;
			};

			class CollisionWithPlane : public ParticlesUpdate
			{
			public:
				CollisionWithPlane() { type = TYPEUP::COLLISIONWITHPLANE; }
				CollisionWithPlane(std::string namePrefab,
					Cookie::Core::Math::Vec3 normal = {0, 1, 0}, float distance = -0.70f)
					: dis(distance), n(normal) { type = TYPEUP::COLLISIONWITHPLANE; }
				~CollisionWithPlane()override {}

				float dis = -0.70f;
				Cookie::Core::Math::Vec3 n = Cookie::Core::Math::Vec3(0, 1, 0);
				std::string namePrefab = "Death_All";

				virtual void Update(ParticlesData* p) override;
			};


			class CreateParticlesFollowing : public ParticlesUpdate
			{
			public:
				ParticlesData* data;
				float coeffScale = 1;
				float coeffPos = 3;
				float time = 0.35f;

				virtual void Update(ParticlesData* p) override;

				CreateParticlesFollowing() { type = TYPEUP::CREATEPARTICLES; }
				CreateParticlesFollowing(ParticlesData& data, float time = 0.35f, float coeff = 1, float coeffPos = 3)
					: data(&data), coeffScale(coeff), coeffPos(coeffPos), time(time) { type = TYPEUP::CREATEPARTICLES; }
				~CreateParticlesFollowing()override {}
			};

			class Shadow : public ParticlesUpdate
			{
			public:
				ParticlesData* data;
				float time = 0.35f;

				virtual void Update(ParticlesData* p) override;

				Shadow() { type = TYPEUP::SHADOW; }
				Shadow(ParticlesData& data, float time = 0.35f)
					: data(&data), time(time) {
					type = TYPEUP::SHADOW;
				}
				~Shadow()override {}
			};

			class SpawnEnd : public ParticlesUpdate
			{
			public:
				Cookie::Core::Math::Vec3 posSpawn;
				std::string namePrefab = "Explosion";

				virtual void Update(ParticlesData* p) override;

				SpawnEnd() { type = TYPEUP::SPAWNEND; }
				~SpawnEnd()override { type = TYPEUP::SPAWNEND; }
			};

			class StopAt : public ParticlesUpdate
			{
			public:
				Cookie::Core::Math::Vec3 posEnd;

				virtual void Update(ParticlesData* p) override;

				StopAt() { type = TYPEUP::STOPAT; }
				~StopAt()override { type = TYPEUP::STOPAT; }
			};
		}
	}
}


#endif // !__PARTICLESUPADATE_HPP__
