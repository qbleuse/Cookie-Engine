#include "Coordinator.hpp"
#include "PlayerData.hpp"
#include "ResourcesManager.hpp"
#include "Texture.hpp"
#include "ActionPanelWidget.hpp"

#include "ComponentHandler.hpp"
#include "Prefab.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Resources;
using namespace Cookie::ECS;


bool ActionPanel::SafeIconButton(const Texture* const & texture, const float size)
{
	static const Texture* const & nullTexture = resources.icons["Assets/EditorUIcons/Log.ico"].get();

	return ImageButton(static_cast<ImTextureID>(texture ? texture->GetResourceView() : nullTexture->GetResourceView()), { size, size });
}

void ActionPanel::ToolTipCost(const Prefab* const & prefab, const Cookie::Gameplay::Income* const & income)
{
	if (IsItemHovered())
	{
		BeginTooltip();

		if (prefab->gameplay.cost.costPrimary)
		{
			TextColored({ 1.f, 0.816f, 0.31f, 1.f }, "%.1f units of wheat", prefab->gameplay.cost.costPrimary);
			SameLine();
			TextColored(income->primary >= prefab->gameplay.cost.costPrimary ? ImVec4{ 0, 1, 0, 1 } : ImVec4{ 1, 0, 0, 1 }, " / %.1f", income->primary);
		}

		if (prefab->gameplay.cost.costSecondary)
		{
			TextColored({ 0.482f, 0.247f, 0.f, 1.f }, "%.1f units of chocolate", prefab->gameplay.cost.costSecondary);
			SameLine();
			TextColored(income->secondary >= prefab->gameplay.cost.costSecondary ? ImVec4{ 0, 1, 0, 1 } : ImVec4{ 1, 0, 0, 1 }, " / %.1f", income->secondary);
		}

		EndTooltip();
	}
}


void ActionPanel::WindowDisplay()
{
	TryBeginWindow(ImGuiWindowFlags_NoBackground)
	{
		ImGui::SetCursorPos({0, 0});
		Image(static_cast<ImTextureID>(resources.icons["Assets/UI/Cadre_Right.png"].get()->GetResourceView()), GetWindowSize());
		ImGui::SetCursorPos(GetStyle().WindowPadding);

		if (coordinator.selectedEntities.size() == 1)
		{
			const Entity* const & selectedEntity = coordinator.selectedEntities[0];
			ComponentGameplay& sEntityGameplayComp = coordinator.componentHandler->GetComponentGameplay(selectedEntity->id);

			const float iconSize = (GetContentRegionAvail().y / 4.f);

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::PRODUCER)
			{
				unsigned short i = 0;
				for (const Prefab* const & pU : sEntityGameplayComp.componentProducer.possibleUnits)
				{
					if (SafeIconButton(pU->model.icon, iconSize))
						sEntityGameplayComp.componentProducer.AddUnitToQueue(i);

					ToolTipCost(pU, sEntityGameplayComp.componentProducer.income);

					i++;

					(i % 3) ? SameLine() : NewLine();
				}

			}

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::WORKER)
			{
				unsigned short i = 0;
				for (Prefab* const& pB : sEntityGameplayComp.componentWorker.possibleBuildings)
				{
					if (SafeIconButton(pB->model.icon, iconSize))
					{
						plData.buildingToBuild			= &pB->gameplay.componentProducer;
						plData.workerWhoBuild			= &sEntityGameplayComp.componentWorker;
						plData.indexOfBuildingInWorker	= i;
					}

					ToolTipCost(pB, sEntityGameplayComp.componentWorker.income);

					i++;

					(i % 3) ? SameLine() : NewLine();
				}

			}
		}
	}

	ImGui::End();
}