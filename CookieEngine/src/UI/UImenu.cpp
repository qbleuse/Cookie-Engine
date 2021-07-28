#include "ResourcesManager.hpp"
#include "UImenu.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UI;
using namespace Cookie::Resources;


UImenu::UImenu(Cookie::Resources::ResourcesManager& _resources):
	    resources	(_resources),
		tittleFont	(GetIO().Fonts->AddFontFromFileTTF("Assets/UI/Roboto-Medium.ttf", 90))
{
}

void UImenu::DisplayMenus() const
{
	const ImVec2 windowSize = GetWindowSize();
	
	if (openMenuFlag)
	{
		OpenPopup("Main menu##GAMEMAINMENU");
		openMenuFlag = false;
	}


	SetNextWindowSize(windowSize);
	SetNextWindowViewport(GetWindowViewport()->ID);

	if (BeginPopupModal("Main menu##GAMEMAINMENU", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove))
	{
		menuState = true;

		Image(static_cast<ImTextureID>(resources.icons["Assets/UI/WallpaperStartGame.tif"]->GetResourceView()), GetContentRegionAvail());


		PushStyleVar(ImGuiStyleVar_FrameRounding, 25.f);

		PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0.85f});
		PushStyleColor(ImGuiCol_ButtonActive, { 0, 0, 0, 1.f });
		PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0.70f });
		

		SetCursorPos({15.f, 20.f});
		
		PushFont(tittleFont);
		TextColored({0, 0, 0, 1}, "Cookie Battle");
		PopFont();


		SetWindowFontScale(1.5f);

		SetCursorPosX(15.f);
		if (Button("START!", { 150.f, 60.f }))
			CloseCurrentPopup();

		SetWindowFontScale(1.f);

		SetCursorPosX(15.f);
		if (Button("Quit", { 100.f, 40.f }))
		{
			quitFunc();
			CloseCurrentPopup();
		}


		PopStyleColor(3);
		PopStyleVar();

		EndPopup();
	}
	else menuState = false;
}