#ifndef __PARTICLE_EDITOR_W_HPP__
#define __PARTICLE_EDITOR_W_HPP__

#include "UIwidgetBases.hpp"
#include "ResourcesManager.hpp"

namespace Cookie::UIwidget
{
	class ParticleEditor final : public WItemBase
	{
	public:
		inline ParticleEditor(Resources::ResourcesManager& manager)
			: WItemBase("Particle editor"), manager(manager)
		{}

		void WindowDisplay() override;

		Resources::ResourcesManager& manager;
	};
}

#endif