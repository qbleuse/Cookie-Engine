#include "Editor.hpp"
#include "UIcore.hpp"

#pragma comment(lib, "dxguid.lib")


extern "C"
{
	__declspec(dllexport) int NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace Cookie;
using namespace Cookie::Core;
using namespace Cookie::Core::Math;

int main()
{
	{
		Core::UIcore::PreInitContext();
		Editor editor;

		editor.Loop();

		Core::UIcore::Terminate();
	}

	return 0;
}
