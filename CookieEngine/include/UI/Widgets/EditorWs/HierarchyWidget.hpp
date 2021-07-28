#ifndef	__HIERARCHY_W_HPP__
#define __HIERARCHY_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Resources { class ResourcesManager; }
namespace Cookie::Resources	{ class Scene; }
namespace Cookie::ECS		{ class Coordinator; }
namespace Cookie			{ struct FocusEntity; }


namespace Cookie::UIwidget
{
	class Inspector;

	/*
		Displays all entities inside the scene.
		Also provides options to completely clear and delete every entity, or to place empty/prefab'ed entities.
	*/
	class Hierarchy final : public WItemBase
	{
		Cookie::Resources::ResourcesManager&		resources;
		std::unique_ptr<Cookie::Resources::Scene>&	scene;
		Cookie::ECS::Coordinator&					coordinator;

		FocusEntity& selectedEntity;

	public:
		inline Hierarchy(Cookie::Resources::ResourcesManager& _resources, std::unique_ptr<Cookie::Resources::Scene>& _scene, Cookie::ECS::Coordinator& _coordinator, FocusEntity& _selectedEntity)
			: WItemBase		("Hierarchy", false),
			  resources     (_resources),
			  scene			(_scene),
			  coordinator	(_coordinator),
			  selectedEntity(_selectedEntity)
		{}

		void WindowDisplay() override;
	};
}

#endif
