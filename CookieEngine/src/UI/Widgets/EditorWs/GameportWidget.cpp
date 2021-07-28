#include "Game.hpp"
#include "GameportWidget.hpp"

#include "Scene.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UIwidget;


bool GamePort::BeginWindow(int windowFlags)
{
	if (isPlaying)
	{	
		contentVisible = ImGui::Begin(windowName, (isPlaying ? nullptr : &opened), windowFlags);

		return true;
	}
	else return false;
}


void GamePort::WindowDisplay()
{
	TryBeginWindow(ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus)
	{
		BeginChild("UnmovingContainer", {(float)game.renderer.window.width, (float)game.renderer.window.height}, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove);

		ImVec2 gameportPos;

		gameportPos.x = GetWindowPos().x + GetCursorPosX();
		gameportPos.y = GetWindowPos().y + GetCursorPosY();

		game.scene->camera->windowOffset = { {gameportPos.x, gameportPos.y} };
		game.scene->camera->Activate();

		ImGui::Image(static_cast<ImTextureID>(game.frameBuffer.shaderResource), { (float)game.renderer.window.width, (float)game.renderer.window.height });
			
		game.scene->uiScene.RenderLayout();

		game.uiMenu.DisplayMenus();

		EndChild();
	}

	ImGui::End();
}
