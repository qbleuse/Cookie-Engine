#ifndef __WORLD_SETTINGS_W_HPP__
#define __WORLD_SETTINGS_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Resources { class Scene; class ResourceManager; }
namespace Cookie::Render	{ struct LightsArray; class SkyBox; }


namespace Cookie::UIwidget
{
	class WorldSettingsWidget final : public WItemBase
	{
		std::unique_ptr<Resources::Scene>&	scene;
		Cookie::Render::LightsArray&		lights;
		Cookie::Render::SkyBox&				skybox;

		Cookie::Resources::ResourcesManager& resources;

	public:
		inline WorldSettingsWidget(std::unique_ptr<Resources::Scene>& _scene, Cookie::Render::LightsArray& _lights, Cookie::Render::SkyBox& _skybox, Cookie::Resources::ResourcesManager& _resources)
			: WItemBase	("World settings", false),
			  scene		(_scene),
			  lights	(_lights),
			  skybox	(_skybox),
			  resources (_resources)
		{}

		void WindowDisplay() override;
	};
}

#endif // !__WORLD_SETTING_W_HPP__
