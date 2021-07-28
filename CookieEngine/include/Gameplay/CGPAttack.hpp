#ifndef _CGP_ATTACK_HPP__
#define _CGP_ATTACK_HPP__

#include "CGPLive.hpp"

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
		class CGPAttack 
		{
		public:
			ECS::ComponentTransform*			   trs       {nullptr};
			Resources::Sound*					   sfxAttack {nullptr};
			Resources::Particles::ParticlesPrefab* vfxAttack {nullptr};

			bool needToAttack    {false};

			float powerLevel     {0}; //will be used for AI analysis
			float attackDamage   {1};
			float attackSpeed    {1}; 
			float attackCooldown {0};
			float attackRange    {10};

			CGPLive* target {nullptr}; 


			CGPAttack() {}
			~CGPAttack() {}

			inline void ToDefault() noexcept
			{
				sfxAttack = nullptr;
				vfxAttack = nullptr;

				needToAttack = false;
				attackDamage = 0;
				attackSpeed = 0;
				attackRange = 0;
				target = nullptr;
			}

			void Attack();

		};


	}
}

#endif // !_CGP_ATTACK_HPP__