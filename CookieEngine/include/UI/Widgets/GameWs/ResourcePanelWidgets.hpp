#ifndef __RESOURCE_PANEL_WS_HPP__
#define __RESOURCE_PANEL_WS_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Resources { class ResourcesManager; class Texture; }
namespace Cookie::Gameplay	{ struct Income; }


namespace Cookie::UIwidget
{
	namespace GameWidgetID
	{ constexpr int IncomePanelID = 1 << 3; }

	/*
		The income panel display multiple player-related infos,
		The resources currently stored, and the population.
	*/
	class IncomePanel final : public GameWindowBase
	{
		const Cookie::Resources::Texture* const panelBg;

		const Cookie::Gameplay::Income& income;

	public:
		IncomePanel(Cookie::Resources::ResourcesManager& _resources, const Cookie::Gameplay::Income& _income);
	
		void WindowDisplay() override;

		inline int GetID() override { return GameWidgetID::IncomePanelID; };
	};
}



#endif // !__RESOURCE_PANEL_WS_HPP__
