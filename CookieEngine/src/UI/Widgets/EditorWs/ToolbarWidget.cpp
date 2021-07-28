#include "Resources/Texture.hpp"
#include "ResourcesManager.hpp"
#include "ToolbarWidget.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UIwidget;


Toolbar::Toolbar(Cookie::Resources::ResourcesManager& _resources, bool& _isPlaying, bool& _isPaused)
	: WindowBase	("##TOOLBAR"),
	  isPlaying		(_isPlaying),
	  isPaused		(_isPaused),

	  icons			{_resources.icons["Assets/EditorUIcons/translate.ico"].get(),
					 _resources.icons["Assets/EditorUIcons/rotate.ico"].get(),
			         _resources.icons["Assets/EditorUIcons/zoom.ico"].get(),
			         _resources.icons["Assets/EditorUIcons/play.ico"].get(),
			         _resources.icons["Assets/EditorUIcons/Stop.ico"].get(),
			         _resources.icons["Assets/EditorUIcons/pause.ico"].get(),
			         _resources.icons["Assets/EditorUIcons/frame.ico"].get()}
{}


bool Toolbar::BeginWindow(int windowFlags)
{
	if (!opened) return false;

	contentVisible = ImGui::BeginChild(windowName, {GetWindowContentRegionWidth(), 50}, true, windowFlags);

	return true;
}

void Toolbar::ButtonsDisplay()
{
	PushStyleColor(ImGuiCol_Button,			{ 0.70, 0.70, 0.70, 1 });
	PushStyleColor(ImGuiCol_ButtonActive,	{ 0.35, 0.35, 0.35, 1 });
	PushStyleColor(ImGuiCol_ButtonHovered,	{ 0.90, 0.90, 0.90, 1 });
	
	PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);


	SameLine(15);

	if (ImageButton(icons[(int)ToolbarIcons::Translator]->GetResourceView(), { 25, 25 }, { 0, 0 }, { 1, 1 }, 5))
	{
		currentTrsfTool = TransformTool::Translate;
	} 
	SameLine(15 + (35 * 1));

	if (ImageButton(icons[(int)ToolbarIcons::Rotator]	->GetResourceView(), { 25, 25 }, { 0, 0 }, { 1, 1 }, 5))
	{
		currentTrsfTool = TransformTool::Rotate;
	}	
	SameLine(15 + (35 * 2));

	if (ImageButton(icons[(int)ToolbarIcons::Scaler]	->GetResourceView(), { 25, 25 }, { 0, 0 }, { 1, 1 }, 5))
	{
		currentTrsfTool = TransformTool::Scale;
	}		
	SameLine(15 + (35 * 3));


	SameLine(GetWindowWidth() / 2);

	if		(!isPlaying && ImageButton(icons[(int)ToolbarIcons::Play]->GetResourceView(), { 25, 25 }, { 0, 0 }, { 1, 1 }, 5))
	{
		isPlaying = true;
	}
	else if ( isPlaying && ImageButton(icons[(int)ToolbarIcons::Stop]->GetResourceView(), { 25, 25 }, { 0, 0 }, { 1, 1 }, 5))
	{
		isPlaying = false;
	}
	SameLine(GetWindowWidth() / 2 + (35 * 1));


	if (ImageButton(icons[(int)ToolbarIcons::Pause]->GetResourceView(), { 25, 25 }, { 0, 0 }, { 1, 1 }, 5))
	{
		isPaused ^= true;
	}
	SameLine(GetWindowWidth() / 2 + (35 * 2));


	if (ImageButton(icons[(int)ToolbarIcons::Frame]->GetResourceView(), { 25, 25 }, { 0, 0 }, { 1, 1 }, 5))
	{
		isPaused = false;
		reqframe = true;
	}


	PopStyleVar(1);
	PopStyleColor(3);
}


void Toolbar::WindowDisplay()
{
	if (reqframe)
	{
		reqframe = false;
		isPaused = true;
	}

	TryBeginWindow(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)
	{
		ButtonsDisplay();
	}

	ImGui::EndChild();
}