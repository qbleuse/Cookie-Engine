#include "UIeditor.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

using namespace ImGui;
using namespace Cookie::UI;


UIeditor::UIeditor()
{
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	io.ConfigViewportsNoDefaultParent = true;
}


void UIeditor::ClearWidgets()
{
	for (std::vector<std::shared_ptr<UIwidget::WindowBase>>& WiVec : UIndows)
	{
		for (std::shared_ptr<UIwidget::WindowBase>& WiW : WiVec)
		{ WiW.reset(); }

		std::vector<std::shared_ptr<UIwidget::WindowBase>>().swap(WiVec);
	}

	for (std::vector<std::shared_ptr<UIwidget::ItemBase>>& ItVec : UItems)
	{
		for (std::shared_ptr<UIwidget::ItemBase>& ItW : ItVec)
		{ ItW.reset(); }

		std::vector<std::shared_ptr<UIwidget::ItemBase>>().swap(ItVec);
	}
}


void UIeditor::UpdateUI()
{
	ImGuizmo::BeginFrame();

	DockSpaceOverViewport(GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	// Main Window space
	{
		BeginMainMenuBar();

		for (int i = 0; i < menus.size(); i++)
		{
			if (ImGui::BeginMenu(menus[i]))
			{
				for (std::shared_ptr<UIwidget::ItemBase>& cI : UItems[i])
				{
					cI->ItemDisplay();
				}

				ImGui::EndMenu();
			}

			for (std::shared_ptr<UIwidget::WindowBase>& cW : UIndows[i])
			{
				cW->WindowDisplay();
			}
		}

		EndMainMenuBar();

		for (std::shared_ptr<UIwidget::WindowBase>& cOW : UIndows[menus.size()])
		{
			cOW->WindowDisplay();
		}
	}
}
