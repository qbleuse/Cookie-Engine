#ifndef __UI_CORE_HPP__
#define __UI_CORE_HPP__

namespace Cookie::Render	{ class Renderer; }


namespace Cookie::Core
{
	// UIcore is a namespace that reunites functions that have for role to manage ImGui on a global level. To initialize and shut it down.
	namespace UIcore
	{
		static bool initialized = false;


		// Call this first to settle in a context. Afterwards you can call in other flags before the next step to finish the initialization.
		void PreInitContext();

		// Finish and confirm the initialization with the current IO-context flags, activating the backends and their selected add-ons.
		void FinishInit(const Cookie::Render::Renderer& _renderer);

		// Shuts down the backends and destroys the context.
		// ImGui should not be used beyond this call.
		void Terminate();


		// Begins a frame for the entirety of the UI.
		void BeginFrame();

		// Ends the overall UI frame.
		void EndFrame();
	};
}

#endif