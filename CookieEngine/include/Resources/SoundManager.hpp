/*#ifndef __SOUNDMANAGER_HPP__
#define __SOUNDMANAGER_HPP__

#include <unordered_map>
#include <memory>
#include "Sound.hpp"

namespace Cookie
{
	namespace Render
	{
		class Camera;
	}

	namespace Resources
	{
		class ResourcesManager;

		class SoundManager
		{
		public:
			static FMOD::System* system;

			static void InitSystem();
			static void Release();
			static void LoadAllMusic(ResourcesManager& resourcesManager);

			static void PlayMusic(Sound* const& sound);
			static void PlayMusic3D(Sound* const& sound, const Cookie::Core::Math::Vec3& pos);

			static void UpdateFMODFor3DMusic(const Cookie::Render::Camera& cam);

			static inline void SetVolume(Sound* const& sound, float vol)
			{ sound->vol = vol; }

			static inline void SetPaused(Sound* const& sound, bool isPaused)
			{ sound->chan->setPaused(isPaused); }

			static inline void Loop(Sound* const& sound)
			{ sound->mode |= FMOD_LOOP_NORMAL; }

			static inline void Normal(Sound* const& sound)
			{ sound->mode -= FMOD_LOOP_NORMAL; }

			static inline void Set3D(Sound* const& sound, const Cookie::Core::Math::Vec3& pos)
			{ 
				sound->mode -= sound->mode & FMOD_2D;
				sound->mode |= FMOD_3D;
				sound->pos = pos;
			}

			static inline void Set3D(Sound* const& sound)
			{ 
				sound->mode -= sound->mode & FMOD_2D;
				sound->mode |= FMOD_3D;
			}

			static inline void SetPosition(Sound* const& sound, const Cookie::Core::Math::Vec3& pos)
			{ sound->pos = pos; }

			static inline void Set2D(Sound* const& sound)
			{ 
				sound->mode -= sound->mode & FMOD_3D;
				sound->mode |= FMOD_2D;
			}

			static inline void SetMode(Sound* const& sound, FMOD_MODE mode)
			{ sound->mode = mode; }
		};
	}
}

#endif
*/