#ifndef	__AIBEHAVIOR_EDITOR_W_HPP__
#define __AIBEHAVIOR_EDITOR_W_HPP__

#include "UIwidgetBases.hpp"
#include "AIBehavior.hpp"

namespace Cookie::Resources { class ResourcesManager; }
namespace Cookie::Gameplay	{ struct AIStep; }


namespace Cookie::UIwidget
{
	class AIBehaviorEditor final : public WItemBase
	{
		Cookie::Resources::ResourcesManager& resources;

		Cookie::Gameplay::AIBehavior		 behaviorToEdit;

		Cookie::Gameplay::AIStep*            selectedStep		= nullptr;
		bool			 					 isEditing			= false;

	public:
		inline AIBehaviorEditor(Resources::ResourcesManager& _resources) 
			: WItemBase	("AIBehavior editor"), 
			  resources	(_resources)
		{}
		
		
		void WindowDisplay() override;
	};
}

#endif