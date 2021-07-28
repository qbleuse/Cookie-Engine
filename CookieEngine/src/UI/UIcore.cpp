#include "Renderer.hpp"
#include "UIcore.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_dx11.h>


using namespace ImGui;
using namespace Cookie::Core;


void UIcore::PreInitContext()
{
	assert((!initialized) && (GetCurrentContext() == nullptr));


	IMGUI_CHECKVERSION();

	CreateContext();

	StyleColorsDark();
	GetStyle().Colors[ImGuiCol_DockingEmptyBg] = { 0, 0, 0, 0 };
	GetIO().Fonts->AddFontDefault();
}

void UIcore::FinishInit(const Cookie::Render::Renderer& _renderer)
{
	assert((!initialized) && (GetCurrentContext() != nullptr));


	ImGui_ImplGlfw_InitNoAPI(_renderer.window.window, true);
	ImGui_ImplDX11_Init(Render::RendererRemote::device, _renderer.remote.context);
	ImGui_ImplDX11_CreateDeviceObjects();

	initialized = true;
}

void UIcore::Terminate()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	DestroyContext();
}


void UIcore::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();
}

void UIcore::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(GetDrawData());

#ifdef IMGUI_HAS_VIEWPORT
	UpdatePlatformWindows();
	RenderPlatformWindowsDefault();
#endif
}