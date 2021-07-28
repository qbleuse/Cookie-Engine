#ifndef __GAMEPORT_W_HPP__
#define __GAMEPORT_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie { class Game; }


namespace Cookie::UIwidget
{
	/*
		The Gameport is the window into the game loop.

		It appears automatically as soon as the game is started and vanishes when the game stops running.
		The Gameport actually simply puts the render of the scene under the game's ui. Input-mouse detection allows movement of camera as well in a RTS manner.
	*/
	class GamePort final : public WindowBase
	{
		const bool& isPlaying;
		const Cookie::Game& game;

	protected:
		bool BeginWindow(int windowFlags = 0) override;
	
	public:
		GamePort(bool& _isPlaying, const Cookie::Game& _game)
			: WindowBase("Gameport", false),
			  isPlaying (_isPlaying),
			  game		(_game)
		{}

		void WindowDisplay() override;
	};
}

#endif