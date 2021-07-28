#ifndef __CONSOLE_W_HPP__
#define __CONSOLE_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Core		{ class DebugMessageHandler; struct DebugMessage; }
namespace Cookie::Resources { class ResourcesManager; class Texture; }


namespace Cookie::UIwidget
{
	/*
		The console uses the debug singleton to display all logs, warnings and errors.
		This also where there is the feature to clean all sent messages, or to group them.
	*/
	class Console final : public WItemBase
	{
		Cookie::Core::DebugMessageHandler& debugManager;
		
		uint8_t errorFlash = 0;

		bool messagesGrouped = false;

		const Cookie::Resources::Texture* const icons[3];

	private:
		// The ungrouped display (default) displays all messages from latest to oldest, each with the date they've been sent.
		void UngroupedDisplay();

		// The grouped display puts all identical messages as one block with the number of repetitions and the date of the latest.
		void GroupedDisplay();

		void DisplayMessage(Cookie::Core::DebugMessage& message);

		// Animates the color depending on what message type it is.
		void MessageColorBounce(unsigned short intensity, uint8_t& colorVariant, bool& bouncing, unsigned short& colorBounces);

	protected:
		bool BeginWindow(int windowFlags = 0) final override;

	public:
		Console(Cookie::Core::DebugMessageHandler& _debugManager, Cookie::Resources::ResourcesManager& _resources);
		Console(const Console&) = delete;
		Console(Console&&)		= default;

		void WindowDisplay() override;
	};
}

#endif