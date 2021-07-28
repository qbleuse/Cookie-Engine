#ifndef __UI_SCENE_HPP__
#define __UI_SCENE_HPP__

#include "UIwidgetBases.hpp"

#include <vector>

namespace Cookie			{ class Game; }
namespace Cookie::Resources { class Scene; }


namespace Cookie::UI
{
	class UIscene
	{
		std::vector<std::unique_ptr<UIwidget::GameWindowBase>> sceneWidgets;
		
		// Prevents widgets from insta-appearing on first frame to not mess up the z-order
		bool firstRoundDone = false;

		// A flag that is true if any of the windows displayed by UIscene is currently hovered by the mouse.
		bool isHovered = false;
		
	public:
		// Keeps track of what widgets have already been added.
		int signature = 0;
	
	public:
		// Simple struct to facilitate UI scene load/save.
		struct GameWindowInfo
		{
			int ID;

			int xPos,  yPos,
				width, height;
		};
	
		UIscene() = default;
		UIscene(const std::vector<GameWindowInfo>& _gameWindows, Cookie::Game& _game);

		~UIscene();

		inline const bool IsHovered() { return isHovered; }

		void								LoadLayout	(const std::vector<GameWindowInfo>& GameWindows, Cookie::Game& game, Cookie::Resources::Scene& scene);
		const std::vector<GameWindowInfo>	SaveLayout	(bool clean = false);

		// Safely clean the entire UI of the scene and free all used memory.
		void CleanLayout();
		
		// Render the current UI of the scene, keeping isHovered updated.
		void RenderLayout();

		inline std::vector<std::unique_ptr<UIwidget::GameWindowBase>>& GetLayout()
		{ return sceneWidgets; }
	};
}

#endif // !__UI_SCENE_HPP__
