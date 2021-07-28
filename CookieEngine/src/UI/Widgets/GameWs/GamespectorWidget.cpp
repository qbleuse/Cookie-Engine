#include "Coordinator.hpp"
#include "ResourcesManager.hpp"
#include "Texture.hpp"
#include "GamespectorWidget.hpp"

#include "ComponentHandler.hpp"
#include "Prefab.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Resources;
using namespace Cookie::ECS;


void Gamespector::SafeIcon(const Texture* const & texture, const float size)
{
	static const Texture* const & nullTexture = resources.icons["Assets/EditorUIcons/Log.ico"].get();

	Image(static_cast<ImTextureID>(texture ? texture->GetResourceView() : nullTexture->GetResourceView()), {size, size}); 
}

void Gamespector::LifeBar(const float currentLife, const float maxLife, const float barWidth, const float barHeight)
{
	const float lifeFrac = currentLife / maxLife;

	static float RGBc[3] = {0, 0, 0};
	ColorConvertHSVtoRGB(0.486f * lifeFrac, 1, 1, RGBc[0], RGBc[1], RGBc[2]);

	PushStyleColor(ImGuiCol_PlotHistogram, { RGBc[0], RGBc[1], RGBc[2], 1 });

	std::string lifeBarOverlay = std::to_string((int)currentLife) + std::string("/") + std::to_string((int)maxLife);
	ProgressBar(lifeFrac, { barWidth, barHeight }, lifeBarOverlay.c_str());

	PopStyleColor();
}


void Gamespector::WindowDisplay()
{
	TryBeginWindow(ImGuiWindowFlags_NoBackground)
	{
		ImGui::SetCursorPos({ 0, 0 });
		Image(static_cast<ImTextureID>(resources.icons["Assets/UI/Cadre_Middle.png"].get()->GetResourceView()), GetWindowSize());
		ImGui::SetCursorPos(GetStyle().WindowPadding);

		if (coordinator.selectedEntities.size() == 1)
		{
			const Entity* const & selectedEntity = coordinator.selectedEntities[0];
			const ComponentGameplay& sEntityGameplayComp = coordinator.componentHandler->GetComponentGameplay(selectedEntity->id);
			

			BeginGroup();

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::LIVE)
			{
				const float regionHeight = GetContentRegionAvail().y;

				SafeIcon(coordinator.componentHandler->GetComponentModel(selectedEntity->id).icon, regionHeight - 19.f);
				
				LifeBar(sEntityGameplayComp.componentLive.lifeCurrent, sEntityGameplayComp.componentLive.lifeMax, regionHeight - 19.f, 15.f);
			}
			else
				SafeIcon(coordinator.componentHandler->GetComponentModel(selectedEntity->id).icon, GetContentRegionAvail().y);

			EndGroup();

			SameLine();
			BeginGroup();

			SetWindowFontScale(2.f);
			TextColored({0.25f, 0.25f, 1.f, 1}, "%s", selectedEntity->name.c_str());
			SetWindowFontScale(1.f);
			
			Separator();

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::PRODUCER)
			{

				if (sEntityGameplayComp.componentProducer.queueOfUnits.size() > 0)
				{
					const std::vector<Prefab*>& unitsQueue = sEntityGameplayComp.componentProducer.queueOfUnits;
					
					SafeIcon(unitsQueue[0]->model.icon, 50.f);

					SameLine();
					BeginGroup();

					Dummy({0.f, 31.f});
					
					PushStyleColor(ImGuiCol_PlotHistogram, { 0, 1, 1, 1 });
					ProgressBar(sEntityGameplayComp.componentProducer.currentCountdown / unitsQueue[0]->gameplay.cost.timeToProduce, { 160.f, 15.f }, std::to_string((int)sEntityGameplayComp.componentProducer.currentCountdown).c_str());
					PopStyleColor();

					EndGroup();

					if (unitsQueue.size() > 1)
					{
						for (size_t i = 1; i < unitsQueue.size(); i++)
						{
							BeginGroup();
							SafeIcon(unitsQueue[i]->model.icon, 40.f);

							if (Button(("X##" + std::to_string(i)).c_str(), { 40.f, 20.f }))
							{
								coordinator.componentHandler->GetComponentGameplay(selectedEntity->id).componentProducer.RemoveUnitFromQueue(i);
								i--;
							}

							EndGroup();

							SameLine();
						}

						NewLine();
					}
					else Dummy({0, 64.f });
				}
				else
				{
					Dummy({ 50.f, 50.f });

					SameLine();
					BeginGroup();

					Dummy({ 0.f, 31.f });

					PushStyleColor(ImGuiCol_PlotHistogram, { 0.5, 0.5, 0.5, 1 });
					ProgressBar(1.f, { 160.f, 15.f }, "No production");
					PopStyleColor();

					EndGroup();

					Dummy({ 0, 64.f });
				}

				Separator();
			}

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::WORKER)
			{
				if (sEntityGameplayComp.componentWorker.BuildingInConstruction)
				{
					Text(sEntityGameplayComp.componentWorker.needTostartBuilding ? "Moving out to start building!" :  "Construction underway..!");

					PushStyleColor(ImGuiCol_PlotHistogram, { 1, 0, 1, 1 });
					ProgressBar(sEntityGameplayComp.componentWorker.constructionCountdown / sEntityGameplayComp.componentWorker.BuildingInConstruction->gameplay.cost.timeToProduce, { 350.f, 15.f }, std::to_string((int)sEntityGameplayComp.componentWorker.constructionCountdown).c_str());
					PopStyleColor();
				}
			}

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::MOVE)
			{
				static const Texture* const & moveIcon = resources.icons["Assets/UI/Deplacement.png"].get();

				SafeIcon(moveIcon, 50.f);

				if (IsItemHovered())
				{
					BeginTooltip();
					Text("Speed: %.1f", sEntityGameplayComp.componentMove.moveSpeed);
					
					if (sEntityGameplayComp.componentMove.isFlying)
						TextColored({0, 1, 0, 1}, "Flying");
					
					EndTooltip();
				}
			}

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::ATTACK)
			{
				if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::MOVE)
					SameLine();


				static const Texture* const & attackIcon = resources.icons["Assets/UI/Attaque.png"].get();
				
				SafeIcon(attackIcon, 50.f);

				if (IsItemHovered())
				{
					BeginTooltip();
					Text("Attack damage: %.1f", sEntityGameplayComp.componentAttack.attackDamage);
					Text("Attack speed: %.1f", sEntityGameplayComp.componentAttack.attackSpeed);
					Text("Attack range: %.1f", sEntityGameplayComp.componentAttack.attackRange);
					EndTooltip();
				}
			}

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::LIVE)
			{
				if (sEntityGameplayComp.signatureGameplay & (CGP_SIGNATURE::ATTACK | CGP_SIGNATURE::MOVE))
					SameLine();


				static const Texture* const& armorIcon = nullptr;

				SafeIcon(armorIcon, 50.f);

				if (IsItemHovered())
				{
					BeginTooltip();
					Text("This unit has an armor of %.1f resistance units", sEntityGameplayComp.componentLive.armor);
					EndTooltip();
				}
			}

			if (sEntityGameplayComp.signatureGameplay & CGP_SIGNATURE::RESOURCE)
			{
				if (sEntityGameplayComp.componentResource.isPrimary)
				{
					TextColored({ 1.f, 0.816f, 0.31f, 1.f }, "Remaining wheat: %.2f units", sEntityGameplayComp.componentResource.resourceReserve);
				}
				else
				{
					TextColored({ 0.482f, 0.247f, 0.f, 1.f }, "Remaining chocolate: %.2f units", sEntityGameplayComp.componentResource.resourceReserve);
				}
			}

			EndGroup();
		}
		else if (coordinator.selectedEntities.size() > 1)
		{
			float size = (int)GetContentRegionAvail().y * 0.75f;
			int iconsPerLines = GetContentRegionAvail().x / (size + 8);

			while ((int)((coordinator.selectedEntities.size() / iconsPerLines) + 1) * (size + 4) > GetContentRegionAvail().y)
			{
				size /= 2.f;
				iconsPerLines = GetContentRegionAvail().x / (size + 8);
			}

			int placedTemp = 0;

			for (Entity* const & e : coordinator.selectedEntities)
			{
				SafeIcon(coordinator.componentHandler->GetComponentModel(e->id).icon, size);

				if (IsItemHovered())
				{
					BeginTooltip();
					Text("%s", e->name.c_str());
					EndTooltip();
				}

				if (IsItemClicked(ImGuiMouseButton_Left))
				{
					std::vector<Entity*>({e}).swap(coordinator.selectedEntities);
					break;
				}

				placedTemp++;

				(placedTemp % iconsPerLines) ? SameLine() : NewLine();
			}
		}
	}

	ImGui::End();
}
