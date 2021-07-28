#ifndef __UI_EDITOR_HPP_
#define __UI_EDITOR_HPP_


#include "UIwidgetBases.hpp"

#include <vector>


namespace Cookie::UI
{
	// UI "moderator" for the entirety of the editor.
	class UIeditor
	{
		const std::vector<const char*> menus = {"File", "Edit", "Windows", "Help"};

		// All window-able elements of the engine, mostly WItems, which are positioned in the 4 first parts. All only-window elements are in the 5th spot.
		std::vector<std::shared_ptr<UIwidget::WindowBase>>	UIndows[5];

		// All stored item elements of the engine UI.
		std::vector<std::shared_ptr<UIwidget::ItemBase>>	UItems[4];
	
	public:
		UIeditor();
		void ClearWidgets();

		// Adds a window inside the UI elements. Do mind, you'll need something to remote-control it if you intend to close it eventually.
		inline void AddWindow (UIwidget::WindowBase* UIW)
		{ UIndows[4].push_back(std::shared_ptr<UIwidget::WindowBase>(UIW)); }

		// Adds an Item that'll be displayed in the main menu bar. They are usually not moving.
		inline void AddItem	(UIwidget::ItemBase* UIT, int i)
		{ UItems[i].push_back(std::shared_ptr<UIwidget::ItemBase>(UIT)); }

		// WItems are considered both items and windows. Clicking the Item transforms it into a window.
		void AddWItem(UIwidget::WItemBase* UIWI, int i)
		{
			if (i <= 3)
			{
				std::shared_ptr<UIwidget::WItemBase> WItemShare(UIWI);

				UIndows[i].push_back(WItemShare);
				UItems [i].push_back(WItemShare);
			}
		}


		/*
			Main rendering function for the overall UI of the editor.

			Displays the main window, as well as all the widgets added within.
		*/
		void UpdateUI();
	};
}

#endif