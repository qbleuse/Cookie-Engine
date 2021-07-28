/*#include "ResourcesManager.hpp"
#include "SoundManager.hpp"
#include "Serialization.hpp"
#include "Camera.hpp"
#include <iostream>
#include <filesystem>


namespace fs = std::filesystem;

using namespace Cookie::Resources;

FMOD::System* SoundManager::system;

void SoundManager::InitSystem()
{
	if (FMOD::System_Create(&system) != FMOD_OK)
	{
		std::cout << "ERROR!";
		return;
	}

	if (system->init(512, FMOD_INIT_NORMAL, 0) != FMOD_OK)
	{
		std::cout << "ERROR!";
		return;
	}
}

void SoundManager::Release()
{
	system->close();
	system->release();
}

void SoundManager::LoadAllMusic(ResourcesManager& resourcesManager)
{
	std::vector<std::string> filesPath;
	std::vector<std::string> MAsset;
	for (const fs::directory_entry& path : fs::directory_iterator("Assets/Music"))
	{
		if (path.path().string().find(".mp3") != std::string::npos)
			filesPath.push_back(path.path().string());
		if (path.path().string().find(".MAsset") != std::string::npos)
			MAsset.push_back(path.path().string());
	}

	for (unsigned int i = 0; i < filesPath.size(); i++)
	{
		std::string& iFile = filesPath.at(i);
		std::replace(iFile.begin(), iFile.end(), '\\', '/');
	}

	for (unsigned int i = 0; i < MAsset.size(); i++)
	{
		std::string& iFile = MAsset.at(i);
		std::replace(iFile.begin(), iFile.end(), '\\', '/');
	}

	FMOD_MODE mode = FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE;

	for (int i = 0; i < filesPath.size(); i++)
	{
		std::size_t pos = filesPath[i].find("c/");
		std::string name = filesPath[i].substr(pos + 2);

		Cookie::Resources::Sound newSound;
		newSound.filepath = filesPath[i];
		newSound.name = name;
		FMOD::Channel* chan = nullptr;
		newSound.chan = chan;
		newSound.mode = mode;

		resourcesManager.sounds[name] = std::make_unique<Cookie::Resources::Sound>(newSound);
	}

	for (int i = 0; i < MAsset.size(); i++)
	{
		std::size_t pos = MAsset[i].find("c/");
		std::string name = MAsset[i].substr(pos + 2);
		pos = name.find(".MAsset");
		std::size_t end = name.length();
		name.replace(pos, end, ".mp3");
		Cookie::Resources::Serialization::Load::LoadVolumAndModeMusic(MAsset[i], resourcesManager.sounds[name].get());
	}
}

void SoundManager::PlayMusic(Sound* const & sound)
{
	bool isAlreadyPlay;
	sound->chan->isPlaying(&isAlreadyPlay);
	if (isAlreadyPlay)
		return;
		
	if (sound->sound == nullptr)
		system->createSound(sound->filepath.c_str(), sound->mode, nullptr, &sound->sound);

	system->playSound(sound->sound, nullptr, false, &sound->chan);
	sound->chan->setVolume(sound->vol);
}

void SoundManager::PlayMusic3D(Sound* const& sound, const Cookie::Core::Math::Vec3& pos)
{
	PlayMusic(sound);

	if (sound->mode & FMOD_3D)
	{
		Cookie::Core::Math::Vec3 posSound = pos;
		FMOD_VECTOR pos = { posSound.x, posSound.y, posSound.z };
		sound->chan->set3DAttributes(&pos, nullptr);
	}
}

void SoundManager::UpdateFMODFor3DMusic(const Render::Camera& cam)
{//Update for 3D Music
	FMOD_VECTOR temp = { cam.pos.x, cam.pos.y, cam.pos.z };
	Cookie::Resources::SoundManager::system->set3DListenerAttributes(0, &temp, nullptr, nullptr, nullptr);
	Cookie::Resources::SoundManager::system->update();
}
*/