#include "ResourcesManager.hpp"
#include "AIStep.hpp"
#include "AIBehaviorEditorWidget.hpp"

#include "Prefab.hpp"
#include "ComponentHandler.hpp"

#include "Serialization.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "CustomImWidget.hpp"

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Resources;
using namespace Cookie::ECS;
using namespace Cookie::Gameplay;


void AIBehaviorEditor::WindowDisplay()
{
	TryBeginWindow()
	{
		//Show existing
		{
			Text("Currently loaded and available AIBehaviors :"); 

			for (std::unordered_map<std::string, std::unique_ptr<AIBehavior>>::iterator aiBehaviorPtr = resources.aiBehaviors.begin(); aiBehaviorPtr != resources.aiBehaviors.end(); ++aiBehaviorPtr)
			{
				if		(isEditing) Text("%s", aiBehaviorPtr->first.c_str());
				else if (Selectable(aiBehaviorPtr->first.c_str()))
				{
					behaviorToEdit = *aiBehaviorPtr->second.get();
					isEditing = true;
				}
			}

			Separator();
		}

		//Edit AIBehavior
		if (isEditing)
		{
			InputText("AIBehavior name", &behaviorToEdit.name);
			NewLine();


			if (TreeNode("Steps"))
			{
				SameLine();
				if (SmallButton("Add##ADD_STEP"))
				{
					behaviorToEdit.steps.push_back(AIStep{});
					selectedStep = &behaviorToEdit.steps.back();
				}

				//Display all Steps, can select one
				for (size_t i = 0; i < behaviorToEdit.steps.size(); i++)
				{
					bool		isISelected = (selectedStep == &behaviorToEdit.steps[i]);
					std::string nameTag		= (isISelected ? "[" : "") + std::to_string(i + 1) + (isISelected ? "]" : "") + "##" + std::to_string(i);
					
					if (Button(nameTag.c_str()))
						selectedStep = &behaviorToEdit.steps[i];

					SameLine();
					if (SmallButton(("Remove##REMOVE_STEP_" + std::to_string(i)).c_str()))
					{
						behaviorToEdit.steps.erase(behaviorToEdit.steps.begin() + i);

						if (isISelected)
							selectedStep = nullptr;
					}
				}

				//Edit Selected Step
				if (selectedStep != nullptr)
				{
					DragInt("##NbOfWorkers", &selectedStep->nbOfWorker, 1.f, 0, 20, "nbOfWorkers : %d");
					DragInt("##NbOfUnits",   &selectedStep->nbOfUnits,  1.f, 0, 20, "nbOfUnits : %d");


					//Display all Building Names
					if (TreeNode("Building Names"))
					{
						SameLine();
						if (SmallButton("Add##ADD_BUILDING")) OpenPopup("Add Building Pop Up");
						if (BeginPopup("Add Building Pop Up"))
						{
							for (std::unordered_map<std::string, std::unique_ptr<Prefab>>::iterator prefabPtr = resources.prefabs.begin(); prefabPtr != resources.prefabs.end(); ++prefabPtr)
							{
								if (prefabPtr->second->signature & C_SIGNATURE::GAMEPLAY &&
									prefabPtr->second->gameplay.signatureGameplay & CGP_SIGNATURE::PRODUCER &&
									Selectable(prefabPtr->second->name.c_str()))
								{
									selectedStep->listOfBuildings.push_back(prefabPtr->second->name);
								}
							}

							EndPopup();
						}

						for (size_t i = 0; i < selectedStep->listOfBuildings.size(); i++)
						{
							Text("%s", selectedStep->listOfBuildings[i].c_str());

							SameLine();
							if (SmallButton(("Remove##REMOVE_BUILDING" + std::to_string(i)).c_str()))
								selectedStep->listOfBuildings.erase(selectedStep->listOfBuildings.begin() + i);
						}

						TreePop();
					}

					Separator();
				}

				TreePop();
			}



			NewLine();

			if (Button("Cancel"))
			{
				behaviorToEdit.Clear();
				selectedStep = nullptr;
				isEditing = false;
			}
			SameLine();

			if (Button("Confirm and save"))
			{
				//Store in ResourcesManager
				resources.aiBehaviors[behaviorToEdit.name] = (std::make_unique<AIBehavior>(behaviorToEdit));
				
				//Serialize
				Cookie::Resources::Serialization::Save::SaveAIBehavior(behaviorToEdit);

				behaviorToEdit.Clear();
				selectedStep = nullptr;
				isEditing = false;
			}
		}
		else
		{
			if (Button("Create new AIBehavior"))
				isEditing = true;
		}
		
		
	}

	ImGui::End();
}
