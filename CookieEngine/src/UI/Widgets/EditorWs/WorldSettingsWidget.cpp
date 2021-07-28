#include "Scene.hpp"
#include "ResourcesManager.hpp"
#include "Light.hpp"
#include "AIBehavior.hpp"
#include "Drawers/Skybox.hpp"
#include "WorldSettingsWidget.hpp"

#include "MapExplorerHelper.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Resources;
using namespace Cookie::Gameplay;


void WorldSettingsWidget::WindowDisplay()
{
	TryBeginWindow()
	{
		if (TreeNode("Map settings"))
		{
			// Scale
			{
				Cookie::Core::Math::Vec3& mapScale = scene->map.trs.scale;

				static float xzMapScale[2] = {mapScale.x, mapScale.z};
		
				Text("X/Z Scale:"); SameLine();
				if (DragFloat2("##SCALE", xzMapScale, 0.25))
				{
					if (xzMapScale[0] > 0 && xzMapScale[1] > 0)
					{
						mapScale = { xzMapScale[0], mapScale.y, xzMapScale[1] };
						scene->map.ScaleHasChanged();
					}
				}

				if (xzMapScale[0] <= 0.01 || xzMapScale[1] <= 0.01)
				{
					PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
					TextWrapped("Please refrain from inputing very low/negative values.");
					PopStyleColor();
				}
			}


			// Tiles
			{
				Cookie::Core::Math::Vec2& mapTilesNb = scene->map.tilesNb;

				static float xzMapTilesNb[2] = {mapTilesNb.e[0], mapTilesNb.e[1]};

				Text("Tiles in X/Z:"); SameLine();
				if (DragFloat2("##TILES", xzMapTilesNb, 1.f, NULL, NULL, "%.0f"))
				{
					if (xzMapTilesNb[0] > 1 && xzMapTilesNb[1] > 1)
					{
						mapTilesNb = {{ xzMapTilesNb[0], xzMapTilesNb[1] }};
						scene->map.TileNbHasChanged();
					}
				}

				if (xzMapTilesNb[0] <= 1 || xzMapTilesNb[1] <= 1)
				{
					PushStyleColor(ImGuiCol_Text, { 1, 0, 0, 1 });
					TextWrapped("Please refrain from inputing very low/negative values.");
					PopStyleColor();
				}
			}


			// Model
			{
				Text("Map albedo:"); SameLine(110);

				ResourceMapExplorer<Texture>("texture", std::move("##MAPTEXTSELECT"), resources.textures2D, scene->map.model.albedo);


				Text("Map normal:"); SameLine(110);

				ResourceMapExplorer<Texture>("normal texture", std::move("##MAPNORMSELECT"), resources.textures2D, scene->map.model.normal);


				ImGui::Custom::TextSnip("Map metallic-Roughness", 9); SameLine(); Text(":"); SameLine(110);

				ResourceMapExplorer<Texture>("metallic-rough texture", std::move("##MAPMRSELECT"), resources.textures2D, scene->map.model.metallicRoughness);
			}

			
			TreePop();
		}


		if (TreeNode("Lights setting"))
		{
			if (TreeNode("World dir light"))
			{
				static Render::DirLight& dirLight = lights.dirLight;

				Text("Direction:");
				DragFloat3("##DIRLIGHTROT", dirLight.dir.e);

				NewLine();
				Text("Casts shadows:"); SameLine(); Checkbox("##DIRSHADOWS", &dirLight.castShadow);

				NewLine();
				Text("Coloration:");
				ColorEdit3("##DIRCOLOR", dirLight.color.e, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueWheel);
				
				TreePop();
			}

			if (TreeNode("Other lights"))
			{
				SliderInt("##POINTLIGHTNB", (int*)&lights.usedPoints, 0, POINT_LIGHT_MAX_NB, "Nb of Light: %d", ImGuiSliderFlags_AlwaysClamp);

				static int lightIndex = 1;
				static Render::PointLight* selectedPtLight = &lights.pointLights[0];

				if (lights.usedPoints > 0)
				{

					if (SliderInt("##POINTLIGHTSELECT", &lightIndex, 1, lights.usedPoints, "Editing light N%d", ImGuiSliderFlags_AlwaysClamp))
					{
						selectedPtLight = &lights.pointLights[lightIndex - 1];
					}

					NewLine();
					Text("Position:");
					DragFloat3("##POINTPOS", selectedPtLight->pos.e, 0.25f, NULL, NULL, "%.2f");

					NewLine();
					Text("Light radius:");
					DragFloat("##POINTRADIUS", &selectedPtLight->radius, 0.5f, NULL, NULL, "%.1f");

					NewLine();
					Text("Coloration:");
					ColorEdit3("##POINTCOLOR", selectedPtLight->color.e, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueWheel);
				}
				TreePop();
			}

			TreePop();
		}
	
		
		if (TreeNode("Skybox settings"))
		{
			Text("Skybox texture:");
			ResourceMapExplorer<Texture>("cubic texture", std::move("##SKYBOXSELECT"), resources.skyboxes, skybox.texture);

			TreePop();
		}

		if (TreeNode("AI settings"))
		{
			static const char* armyNames[] = { "Default", "Player", "AI1" };

			for (int i = 0; i < scene->armyHandler.livingArmies; ++i)
			{
				Gameplay::E_ARMY_NAME currentArmyName = scene->armyHandler.armies[i].name;
				Text("%s", armyNames[currentArmyName]);

				//if an AI army
				if (currentArmyName > Gameplay::E_ARMY_NAME::E_PLAYER)
				{
					//if there is already a coordinator
					ArmyCoordinator* currentArmyCoordinator = scene->armyHandler.GetArmyCoordinator(currentArmyName);
					if (currentArmyCoordinator)
					{
						SameLine();
						if (SmallButton("Remove##REMOVE_COORDINATOR")) 
							scene->armyHandler.RemoveArmyCoordinator(currentArmyName);

						ResourceMapExplorer<AIBehavior>("AIBehavior", std::move("##AIBEHAVIOR"), resources.aiBehaviors, currentArmyCoordinator->behavior);

					}
					else
					{
						SameLine();
						if (SmallButton("Add##ADD_COORDINATOR")) 
							scene->armyHandler.AddArmyCoordinator(currentArmyName, nullptr);
					}
				}
				else
				{
					SameLine();
					Text(" Can't add an AI");
				}

				NewLine();
			}


			TreePop();
		}
	}

	ImGui::End();
}