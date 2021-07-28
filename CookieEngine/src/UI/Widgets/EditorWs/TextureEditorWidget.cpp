#include <string>
#include "TextureEditorWidget.hpp"

#include "Serialization.hpp"
#include "Resources/Texture.hpp"
#include "Resources/ResourcesManager.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "CustomImWidget.hpp"

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Resources;


void TextureEditor::WindowDisplay()
{
	TryBeginWindow()
	{
		//Text("Currently loaded and available textures:");
		
		
		/*BeginGroup();

        for (std::unordered_map<std::string, std::unique_ptr<Texture>>::iterator textPtr = resources.textures.begin(); textPtr != resources.textures.end(); textPtr++)
        {
			if (textPtr->second && textPtr->second->desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
			{
				BeginGroup();
				Custom::Zoomimage(static_cast<ImTextureID>(textPtr->second->GetResourceView()), 100, 100, 10, true);

				if (newTexture.name == textPtr->second->name)
				{
					TextColored({ 0.75, 0.25, 0.25, 1 }, "%s (<- Already in use!)", textPtr->second->name.c_str());
					newTexture.invalidName = true;
				}
				else Custom::TextSnip(textPtr->second->name.c_str(), 15);

				EndGroup();
			}

			if (BeginPopupContextItem("Texture interaction popup"))
			{
				if (Selectable("Delete this texture!"))
				{ resources.textures.erase(textPtr); }

				EndPopup();
			}


			SameLine();
			if (GetContentRegionAvail().x < 100.f) NewLine();
        }

		EndGroup();*/


		Separator();

		if (!newTexture.creating)
		{
			if (Button("Create new Texture")) 
				newTexture.creating = true;
		}
		else
		{
			InputText("Texture name", &newTexture.name);
			ColorPicker4("##NEW_TEXTURE_COLOR", newTexture.color.e, ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_DisplayRGB);

			NewLine();
			
			
			if (Button("Cancel"))
			{
				newTexture.creating = false;
				newTexture.name.clear();
				newTexture.invalidName = false;
				newTexture.color = { 0, 0, 0, 1 };
			}

			SameLine();
			
			if (newTexture.name.empty())
			{
				Text("Your texture has no name!");
			}
			else if (newTexture.invalidName)
			{
				Text("Your texture's name is invalid. (already in use)");
				newTexture.invalidName = false;
			}
			else if (Button("Confirm and save"))
			{				
				resources.textures2D[newTexture.name] = (std::make_unique<Texture>(newTexture.name, newTexture.color));
				Cookie::Resources::Serialization::Save::SaveTexture(newTexture.name, newTexture.color);

				newTexture.creating = false;
				newTexture.name.clear();
				newTexture.color = { 0, 0, 0, 1 };
			}
		}
	}

	ImGui::End();
}
