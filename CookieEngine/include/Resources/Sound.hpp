/*#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "fmod.hpp"
#include "Vec3.hpp"

namespace Cookie
{
	namespace Resources
	{
		class Sound
		{
		public:
			FMOD::Sound*				sound	= nullptr;
			FMOD::Channel*				chan = nullptr;
			FMOD_MODE					mode = FMOD_DEFAULT;
			Cookie::Core::Math::Vec3	pos;
			std::string					filepath;
			std::string					name;
			float						vol = 1;
		};
	}
}

#endif // !__SOUND_HPP__*/
