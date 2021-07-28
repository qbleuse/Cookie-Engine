#ifndef __MINIMAP_W_HPP__
#define __MINIMAP_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Resources { class Map; class Texture; class ResourcesManager; }
namespace Cookie::Render	{ class FrameBuffer; class Camera; }


namespace Cookie::UIwidget
{
	namespace GameWidgetID
	{ constexpr int MinimapID = 1 << 2; }

	/*
		A rather large display that shows not only the state of the map, space token with buildings and such, but also has markers for teams.
		The current view is outlined on said map with a white square.
		Clicking on the minimap moves the camera/view where it is requested.
	*/
	class Minimap final : public GameWindowBase
	{
		const Cookie::Resources::Texture* const minimapBg;

		const Cookie::Render::FrameBuffer& minimapBuffer;

		Cookie::Render::Camera* const camera;
		const Cookie::Resources::Map&	map;

	public:
		Minimap(Cookie::Resources::ResourcesManager& _resources, const Cookie::Render::FrameBuffer& _minimapBuffer, Cookie::Render::Camera* const _camera, const Cookie::Resources::Map& _map);

		void WindowDisplay() override;

		inline int GetID() override { return GameWidgetID::MinimapID; };
	};
}

#endif // !__MINIMAP_W_HPP__
