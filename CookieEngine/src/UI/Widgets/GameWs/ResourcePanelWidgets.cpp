#include "ResourcesManager.hpp"
#include "Income.hpp"
#include "ResourcePanelWidgets.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Gameplay;


IncomePanel::IncomePanel(Cookie::Resources::ResourcesManager& _resources, const Cookie::Gameplay::Income& _income)
		   : GameWindowBase	("Income panel", true),
		     panelBg		(_resources.icons["Assets/UI/Bouton.png"].get()),
		     income			(_income)
{}

void IncomePanel::WindowDisplay()
{
	TryBeginWindow(ImGuiWindowFlags_NoBackground)
	{
		ImGui::SetCursorPos({ 0, 0 });
		Image(static_cast<ImTextureID>(panelBg->GetResourceView()), GetWindowSize());
		ImGui::SetCursorPos(GetStyle().WindowPadding);

		Text("Wheat: %.1f", income.primary);
		Text("Chocolate: %.1f", income.secondary);
		Text("Population: %.0f / %.0f", income.supplyCurrent, income.supplyMax);
	}

	ImGui::End();
}