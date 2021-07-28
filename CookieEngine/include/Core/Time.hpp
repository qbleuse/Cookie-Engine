#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <ImGui/imgui.h>

namespace Cookie
{
	namespace Core
	{
		static float timeScale {1.f};
		static inline float DeltaTime(){ return ImGui::GetIO().DeltaTime * timeScale; }
		static inline float UnscaledDeltaTime() { return ImGui::GetIO().DeltaTime; };
	}
}



#endif // !__TIME_HPP__
