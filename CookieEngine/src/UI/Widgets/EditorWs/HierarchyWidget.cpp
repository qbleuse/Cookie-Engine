#include "Editor.hpp"
#include "InspectorWidget.hpp"
#include "Scene.hpp"
#include "HierarchyWidget.hpp"

#include "MapExplorerHelper.hpp"

#include <imgui.h>

#include "Serialization.hpp"

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::ECS;
using namespace Cookie::Resources;


void Hierarchy::WindowDisplay()
{
    TryBeginWindow(ImGuiWindowFlags_HorizontalScrollbar)
    {
        if (BeginPopupContextWindow("Out-object hierarchy menu"))
        {
            if (Selectable("Create empty"))
            {
                coordinator.AddEntity(C_SIGNATURE::EMPTY_C);
            }

            {
                Prefab* prefab = ResourceMapSelector<Prefab>("prefab", "##PREFABEDSELECTOR", resources.prefabs);
                if(prefab)
                    coordinator.AddEntity(prefab);
            }

            NewLine();
            if (Selectable("Clean scene"))
            {
                while (coordinator.entityHandler->livingEntities)
                    coordinator.RemoveEntity(coordinator.entityHandler->entities[0]);
            }

            EndPopup();
        }


        TextColored({0.75, 0.75, 0.75, 1}, "%s", scene->name.c_str());


        Separator();

        EntityHandler& entityHandler = *coordinator.entityHandler;
        for (size_t i = 0; i < entityHandler.livingEntities; i++)
        {
            std::string entityNameTag = entityHandler.entities[i].name + "##" + std::to_string(i);

            if (Selectable(entityNameTag.c_str()))
            {
                selectedEntity.toChangeEntityIndex = i;
                //populate
            }

            if (BeginPopupContextItem(entityNameTag.c_str()))
            {
                if (Selectable("Delete entity"))
                    coordinator.RemoveEntity(entityHandler.entities[i]);

                if (Selectable("Save as prefab"))
                {
                    resources.CreateNewPrefabs(entityHandler.entities[i], *coordinator.componentHandler);
                    Cookie::Resources::Serialization::Save::SavePrefab(resources.prefabs[entityHandler.entities[i].namePrefab].get());
                }
                
            
                EndPopup();
            }
        }

        NewLine();
        NewLine();
        NewLine();
    }

    ImGui::End();
}
