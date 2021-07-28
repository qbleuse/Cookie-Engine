#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Render/Renderer.hpp" 

#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Resources/Prefab.hpp"
//#include "Resources/Sound.hpp"

#include "Resources/ResourcesManager.hpp"

#include "ECS/Coordinator.hpp"
#include "Resources/Particles/ParticlesHandler.hpp"
#include "PlayerData.hpp"

#include "UImenu.hpp"

#include <memory>

namespace Cookie
{
	namespace Resources
	{
		class Scene;
	}

	#define MINIMUM_SELECTION_QUAD_LENGTH 1

	class Game
	{
		private:
		public:
			Gameplay::PlayerData                            playerData;
			Render::Renderer								renderer;
			Resources::ResourcesManager						resources;
			ECS::Coordinator								coordinator;
			Render::FrameBuffer								frameBuffer;
			Render::FrameBuffer								miniMapBuffer;
			Resources::Particles::ParticlesHandler			particlesHandler;
			UI::UImenu										uiMenu;

			std::unique_ptr<Resources::Scene>				scene{nullptr};

			bool isPaused = false;

		public:
			 Game(bool windowed = true);
			~Game();

			void Start();
			void Update();
			void Loop();

			//In Loop
			void CalculateMousePosInWorld();
			void HandleGameplayInputs();
			void InputFocusCam();
			void CheckIfBuildingValid();
			void InputCancelBuilding();
			void InputValidateBuilding();
			void InputStartSelectionQuad();
			void InputEndSelectionQuad();
			void InputMoveSelected();
			void InputSetNewEntityDestination();
			void InputSetResourceToWorkers();
			void InputStartBuilding(int index);
			void InputAddUnit(int index);
			void DisplayNewEntityDestination();
			void ECSCalls();
			void DisplayLife();

			void SetScene();
			void SetCamClampFromMap();
			void TryResizeWindow();
	};
}

#endif // !__GAME_HPP__
