#ifndef	__INSPECTOR_W_HPP__
#define __INSPECTOR_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Resources  { class ResourcesManager; }
namespace Cookie::ECS		 { class Coordinator; class Entity; }
namespace Cookie::Core::Math { union Vec2; }
namespace Cookie			 { struct FocusEntity; }
namespace reactphysics3d	 { class Collider; }


namespace Cookie::UIwidget
{
	// The inspector gives an immense multitude of options to edit any selected entity to your liking.
	class Inspector final : public WItemBase
	{
		FocusEntity&			selectedEntity;
		Cookie::ECS::Entity*	recordedEntity = nullptr;
	
		Cookie::Resources::ResourcesManager& resources;
		Cookie::ECS::Coordinator& coordinator;

		reactphysics3d::Collider* selectedCollider = nullptr;


	private:
		// Starter of the entity analysis. Determines what component it has and allows sub-sections to display, and also allows to add new components.
		void EntityInspection();

		// Basic interface for the Transform:
		// Allows live editing of the position, rotation, and scale.
		void TransformInterface();

		// Allows live editing of the current model used by the entity, as well as it's texture.
		void ModelInterface();

		/*
			Advanced interface:
				- Allows to edit colliders after selected from the display list,
				- Allows to add/remove colliders,
				- Provides many options to edit the rigibody/entity as well.

			Fair warning for your sanity: the interface is very, very long. Refer yourself to the other comments to know where you are inside!
		*/
		void PhysicsInterface();

		// Allows live/hot-swapping of the scripts in-use by the entity.
		void ScriptInterface();
		
		// Allows to interface with three inner components corresponding to the entity's health and armor, movement capacities and attack abilities.
		void GameplayInterface();

		// Gives plenty of options to edit the live sounds inside the game, on entities.
		void FXInterface();

	public:
		inline Inspector(FocusEntity& _selectedEntity, Cookie::Resources::ResourcesManager& _resources, Cookie::ECS::Coordinator& _coordinator)
			: WItemBase			("Inspector", false),
			  selectedEntity	(_selectedEntity),
			  resources			(_resources),
			  coordinator		(_coordinator)
		{}

		void WindowDisplay() override;
	};
}

#endif