#ifndef __RESOURCE_EXPLOERER_HELPER_HPP__
#define __RESOURCE_EXPLOERER_HELPER_HPP__

#include <unordered_map>
#include <memory>
#include <algorithm>

#include "Texture.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "CustomImWidget.hpp"


namespace Cookie::Resources
{
	namespace StringHelper
	{
		static size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0)
		{
			// Convert complete given String to lower case
			std::transform(data.begin(), data.end(), data.begin(), ::tolower);

			// Convert complete given Sub String to lower case
			std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);
			
			// Find sub string in given string
			return data.find(toSearch, pos);
		}
	}


	template <class C>
	static void SelectableResourceDisplay(const typename std::unordered_map<std::string, std::unique_ptr<C>>::iterator& It, const std::string& researchString, C*& currentR, const bool is_selected = false)
	{
		if ((StringHelper::findCaseInsensitive(It->second->name, researchString) != std::string::npos) && ImGui::Selectable(It->second->name.c_str(), is_selected))
		{
			currentR = It->second.get();
		}
	}

	// Textures have a special preview feature, with a zoom-in by hovering on the preview.
	//template <>
	//void SelectableResourceDisplay<Texture>(const std::unordered_map<std::string, std::unique_ptr<Texture>>::iterator& textIt, const std::string& researchString, Texture*& currentR, const bool is_selected)
	//{
	//	if (StringHelper::findCaseInsensitive(textIt->second->name, researchString) != std::string::npos)
	//	{
	//		if (textIt->second->desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
	//		{
	//			ImGui::Custom::Zoomimage(static_cast<ImTextureID>(textIt->second->GetResourceView()), 25, 25, 5);
	//
	//			ImGui::SameLine();
	//		}
	//
	//		if (ImGui::Selectable(textIt->second->name.c_str(), is_selected))
	//			currentR = textIt->second.get();
	//	}
	//}


	// Template function used when it is required to go through an entire map of the resource manager to modify a particular component/member.
	// Provides a user-friendly scroll-down menu, a search bar at the top, and an option to clear what is currently in the component/member at the bottom.
	template <class C>
	void ResourceMapExplorer(const std::string&& typeName, const char*&& TAG, std::unordered_map<std::string, std::unique_ptr<C>>& resourceMap, C*& currentR)
	{
		static std::string researchString;

		if (ImGui::BeginCombo(TAG, currentR != nullptr ? currentR->name.c_str() : ("No " + typeName + " applied").c_str(), ImGuiComboFlags_HeightLarge))
		{
			ImGui::InputText((typeName + " search").c_str(), &researchString, ImGuiInputTextFlags_AutoSelectAll);

			ImGui::NewLine();

			for (typename std::unordered_map<std::string, std::unique_ptr<C>>::iterator It = resourceMap.begin(); It != resourceMap.end(); It++)
			{
				if (It->second)
				{
					const bool is_selected = (currentR != nullptr && currentR->name == It->second->name);

					SelectableResourceDisplay<C>(It, researchString, currentR, is_selected);

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::NewLine();

			if (currentR != nullptr)
			{
				if (ImGui::Selectable(("Clear current " + typeName).c_str()))
					currentR = nullptr;
			}
			else ImGui::TextDisabled("Clear current %s", typeName.c_str());

			ImGui::EndCombo();
		}

		if (ImGui::IsItemClicked())
			researchString.clear();
	}


	// Template function used when it is required to select a unique resource inside a map without modifying a component/member. If one is selected it is retured, otherwise it just returns a nullptr.
	// Provides a user-friendly scroll-down menu, and search bar at the top.
	template <class C>
	static C* const ResourceMapSelector(const std::string&& typeName, const char* TAG, std::unordered_map<std::string, std::unique_ptr<C>>& resourceMap)
	{
		static std::string researchString;

		C* selectedResource = nullptr;

		
		if (ImGui::BeginCombo(TAG, ("Select a " + typeName).c_str(), ImGuiComboFlags_HeightLarge))
		{
			ImGui::InputText((typeName + " search").c_str(), &researchString, ImGuiInputTextFlags_AutoSelectAll);

			ImGui::NewLine();

			for (typename std::unordered_map<std::string, std::unique_ptr<C>>::iterator It = resourceMap.begin(); It != resourceMap.end(); It++)
			{
				SelectableResourceDisplay<C>(It, researchString, selectedResource);
			}

			ImGui::EndCombo();
		}

		if (ImGui::IsItemClicked())
			researchString.clear();

		return selectedResource;
	}
}

#endif // !__RESOURCE_EXPLOERER_HELPER_HPP__
