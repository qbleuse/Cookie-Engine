#include "FrameBuffer.hpp"
#include "Camera.hpp"
#include "Map.hpp"
#include "ResourcesManager.hpp"
#include "MinimapWidget.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UIwidget;


Minimap::Minimap(Cookie::Resources::ResourcesManager& _resources, const Cookie::Render::FrameBuffer& _minimapBuffer, Cookie::Render::Camera* const _camera, const Cookie::Resources::Map& _map)
	: GameWindowBase("Minimap", true),
	  minimapBg		(_resources.icons["Assets/UI/Cadre_Left.png"].get()),
	  minimapBuffer	(_minimapBuffer),
	  camera		(_camera),
	  map			(_map)
{}

void Minimap::WindowDisplay()
{
	TryBeginWindow(ImGuiWindowFlags_NoBackground)
	{
		ImGui::SetCursorPos({ 0, 0 });
		Image(static_cast<ImTextureID>(minimapBg->GetResourceView()), GetWindowSize());
		ImGui::SetCursorPos(GetStyle().WindowPadding);

		Image(static_cast<ImTextureID>(minimapBuffer.shaderResource), GetContentRegionAvail());

		if (IsItemHovered() && GetIO().MouseDown[0])
		{
			const ImVec2& mousePos = GetIO().MousePos;

			camera->pos.x = ((mousePos.x - GetWindowPos().x) / GetWindowSize().x) * map.trs.scale.x - (map.trs.scale.x * 0.5);
			camera->pos.z = ((mousePos.y - GetWindowPos().y) / GetWindowSize().y) * map.trs.scale.z - (map.trs.scale.z * 0.5);
		}
	}

	ImGui::End();
}
