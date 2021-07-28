#ifndef	__TOOLBAR_W_HPP__
#define __TOOLBAR_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Resources { class ResourcesManager; class Texture; }


namespace Cookie::UIwidget
{	
	enum class TransformTool
	{ Translate, Rotate, Scale };

	/*
		Allows to switch between the editor tools (translate, rotate, scale),
		And to play/stop/pause the game, or request a single frame to pass.
	*/
	class Toolbar final : public WindowBase
	{		
		TransformTool currentTrsfTool = TransformTool::Translate;

		bool& isPlaying;
		bool& isPaused;
		bool  reqframe = false;

	
		enum class ToolbarIcons
		{ Translator, Rotator, Scaler, Play, Stop, Pause, Frame, ICOS_NUM };

		const Cookie::Resources::Texture* const icons[(int)ToolbarIcons::ICOS_NUM];

	private:
		// Displays all the available tools and playflow options, can be interacted with at all times of course.
		void ButtonsDisplay();

		// Based on the 3 playflow booleans, the return determines the frame value of "isPlaying".
		bool ProcessPlayFlow();

	protected:
		bool BeginWindow(int windowFlags = 0) override;

	public:
		Toolbar(Cookie::Resources::ResourcesManager& _resources, bool& _isPlaying, bool& _isPaused);

		void WindowDisplay() override;


		inline const TransformTool& CurrentTrsfTool() const
		{ return currentTrsfTool; }
	};
}

#endif
