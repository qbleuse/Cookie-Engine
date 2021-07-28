#include "SoundOrchestratorWidget.hpp"

#include "SoundManager.hpp"

#include "MapExplorerHelper.hpp"

#include "Serialization.hpp"

#include <imgui.h>

using namespace Cookie::UIwidget;
using namespace Cookie::Resources;
using namespace ImGui;


void SoundOrchestrator::WindowDisplay()
{
	TryBeginWindow()
	{
		static Sound* selectedSound = nullptr;

		ResourceMapExplorer<Sound>("sound", std::move("##SOUNDESELECTOR"), manager.sounds, selectedSound);

		NewLine();

		if (selectedSound != nullptr)
		{
			static bool pausedSound = true;
			
			if (Selectable("Play this sound") && pausedSound)
			{
				bool isAlreadyPlay;
				selectedSound->chan->isPlaying(&isAlreadyPlay);
				if (isAlreadyPlay)
					SoundManager::SetPaused(selectedSound, false);
				else
					SoundManager::PlayMusic(selectedSound);
				pausedSound = false;
			}

			if (Selectable(pausedSound ? "Resume sound" : "Pause sound"))
			{
				pausedSound ^= 1;
				SoundManager::SetPaused(selectedSound, pausedSound);
			}


			NewLine();
			Text("Sound adjustments:"); 
			NewLine();

			Text("Volume:"); SameLine(100);
			SliderFloat("##MP3VOLUME", &selectedSound->vol, 0.f, 1.f, "%.2f");

			Text("Loops:"); SameLine(100);
			if (RadioButton("##MP3LOOP", selectedSound->mode & FMOD_LOOP_NORMAL))
			{ selectedSound->mode ^= FMOD_LOOP_NORMAL; }

			Text("Is in 3D:"); SameLine(100);
			if (RadioButton("##MP33D", selectedSound->mode & FMOD_3D))
			{
				if		(selectedSound->mode & FMOD_3D)
					SoundManager::Set2D(selectedSound);

				else
					SoundManager::Set3D(selectedSound);
			}

			if (selectedSound->mode & FMOD_3D)
			{
				Text("Position:"); SameLine(100);
				DragFloat3("##MP3POS", selectedSound->pos.e, 0.25f, NULL, NULL, "%.2f");
			}


			NewLine(); NewLine();
			if (Selectable("Overwrite existing parameters"))
			{ Serialization::Save::SaveVolumAndModeMusic(selectedSound); }
		}
	}

	ImGui::End();
}