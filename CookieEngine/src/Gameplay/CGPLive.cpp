#include "Gameplay/CGPLive.hpp" 
#include "SoundManager.hpp"
#include "ParticlesHandler.hpp"
#include <cmath>

using namespace Cookie::Core::Math;
using namespace Cookie::Gameplay;
using namespace Cookie::Resources;
using namespace Cookie::Resources::Particles;


void CGPLive::TakeHit(const float damage)
{
	if(sfxHit)
		SoundManager::PlayMusic3D(sfxHit, trs->pos);
	if(vfxHit)
		ParticlesHandler::CreateParticlesWithPrefab(trs->pos, vfxHit);

	//avoid to gain life if armor exceed damage
	lifeCurrent -= std::fmax(damage - armor, 0);
}