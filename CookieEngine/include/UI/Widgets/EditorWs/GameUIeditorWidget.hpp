#ifndef __GAME_UI_EDITOR_W_HPP__
#define __GAME_UI_EDITOR_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie { class Game; }


namespace Cookie::UIwidget
{
	/*
		Allows to place UI widgets elements that are destined to appear in the game.
		it provides a large canvas the same size as the window itself for placement.

		Holding shift will resize the edited widget into a square.
	*/
	class GameUIeditor final : public WItemBase
	{
		Cookie::Game& game;

	public:
		inline GameUIeditor(Cookie::Game& _game)
			: WItemBase	("UI editor"),
			  game		(_game)
		{}

		void WindowDisplay() override;
	};
}

#endif