#include "Gameplay/CGPAttack.hpp"
#include "SoundManager.hpp"
#include "ParticlesHandler.hpp"
#include "Core/Time.hpp"

using namespace Cookie::Gameplay;
using namespace Cookie::Resources;
using namespace Cookie::Resources::Particles;



void CGPAttack::Attack()
{
	//add Check so attackCooldown will not have high negative value if unit stand still long enough
	if (attackCooldown > 0)
		attackCooldown -= Core::DeltaTime();

	if (target && attackCooldown <= 0)
	{
		if (sfxAttack)
			SoundManager::PlayMusic3D(sfxAttack, trs->pos);
		if (vfxAttack)
			ParticlesHandler::CreateParticlesWithPrefab(trs->pos, vfxAttack, target->trs->pos);

		attackCooldown = attackSpeed;
		target->TakeHit(attackDamage);
	}
}