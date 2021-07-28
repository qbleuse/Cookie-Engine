#include "Debug.hpp"
#include "Resources/Texture.hpp"
#include "ResourcesManager.hpp"
#include "ConsoleWidget.hpp"

#include <imgui.h>

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Core;


Console::Console(DebugMessageHandler& _debugManager, Cookie::Resources::ResourcesManager& _resources)
	: WItemBase		("Console", false),
	  debugManager	(_debugManager),

	  icons			{_resources.icons["Assets/EditorUIcons/Log.ico"].get(),
					 _resources.icons["Assets/EditorUIcons/Warning.ico"].get(),
					 _resources.icons["Assets/EditorUIcons/Error.ico"].get()}
{}


bool Console::BeginWindow(int windowFlags)
{
	if (!opened) return false;

	if (errorFlash > 0)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 1, (255.f - errorFlash) / 255.f, (255.f - errorFlash) / 255.f, 1 });
		contentVisible = ImGui::Begin(windowName, &opened, windowFlags);
		ImGui::PopStyleColor();
	}
	else
	{
		contentVisible = ImGui::Begin(windowName, &opened, windowFlags);
	}

	if (!opened) visible = true;

	return true;
}


void Console::WindowDisplay()
{
	TryBeginWindow(0)
	{
		if (Button("Clear list"))
		{
			std::vector<DebugMessage>().swap(debugManager.storedMessages); 
			errorFlash = 0;
		}

		SameLine();

		if (Button(messagesGrouped ? "Ungroup messages" : "Group messages"))
		{ messagesGrouped = !messagesGrouped; }

		if (BeginTable("Console disps", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersH))
		{
			if		(messagesGrouped == true)	GroupedDisplay();
			else								UngroupedDisplay();

			EndTable();
		}
	}

	ImGui::End();
}


void Console::UngroupedDisplay()
{
	bool latestErrorEncountered = false;

	for (std::vector<DebugMessage>::reverse_iterator it = debugManager.storedMessages.rbegin(); it != debugManager.storedMessages.rend(); ++it)
	{
		DisplayMessage(*it);

		if (!latestErrorEncountered && it->messageType == DebugMessage::Error)
		{
			errorFlash = it->colorVariant;
			latestErrorEncountered = true;
		}
	}
}

void Console::GroupedDisplay()
{
	std::vector<DebugMessage*> firstOccurences;
	std::vector<unsigned int> repetitions;
	bool isNew;

	for (std::vector<DebugMessage>::reverse_iterator it = debugManager.storedMessages.rbegin(); it != debugManager.storedMessages.rend(); ++it)
	{
		isNew = true;

		for (size_t fo = 0; fo < firstOccurences.size(); fo++)
		{
			if ((it->text == firstOccurences[fo]->text) && (it->messageType == firstOccurences[fo]->messageType))
			{
				isNew = false;
				repetitions[fo]++;
			}
		}

		if (isNew)
		{
			firstOccurences.push_back(&*it);
			repetitions.push_back(0);
		}
	}


	for (size_t i = 0; i < firstOccurences.size(); i++)
	{
		DisplayMessage(*firstOccurences[i]);

		if (repetitions[i] > 0)
		{
			TextDisabled("Repeated +%d times", repetitions[i]);
		}
	}
}


void Console::DisplayMessage(DebugMessage& message)
{
	TableNextRow();
	TableNextColumn();
	Image(icons[(int)message.messageType]->GetResourceView(), { 35, 35 });
	TableNextColumn();
	TextDisabled("%s", message.timestamp);
	SameLine();

	switch (message.messageType)
	{
	case (DebugMessage::Log):
		TextColored({ (255.f - message.colorVariant) / 255.f, 1, 1, 1 }, "%s", message.text.c_str());
		MessageColorBounce(1, message.colorVariant, message.bouncing, message.colorBounces);
		break;

	case (DebugMessage::Warning):
		TextColored({ 1, 1, (255.f - message.colorVariant) / 255.f, 1 }, "%s", message.text.c_str());
		MessageColorBounce(3, message.colorVariant, message.bouncing, message.colorBounces);
		break;

	case (DebugMessage::Error):
		TextColored({ 1, (255.f - message.colorVariant) / 255.f, (255.f - message.colorVariant) / 255.f, 1 }, "%s", message.text.c_str());
		MessageColorBounce(5, message.colorVariant, message.bouncing, message.colorBounces);
		break;
	}
}

void Console::MessageColorBounce(unsigned short intensity, uint8_t& colorVariant, bool& bouncing, unsigned short& colorBounces)
{
	if ((!bouncing) && (colorVariant > 0))
	{
		colorVariant -= intensity;

		if (colorVariant <= 0)
		{
			colorVariant = 0;

			if (colorBounces > 0)
			{
				bouncing = true;
				colorBounces--;
			}
		}
	}

	if (bouncing)
	{
		colorVariant += intensity;

		if (colorVariant >= 255)
		{
			colorVariant = 255;

			bouncing = false;
		}
	}
}