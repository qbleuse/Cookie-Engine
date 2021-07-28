#ifndef	__TEXTURE_EDITOR_W_HPP__
#define __TEXTURE_EDITOR_W_HPP__

#include "UIwidgetBases.hpp"
#include "Core/Math/Vec4.hpp"

namespace std				 { using string = basic_string<char, char_traits<char>, allocator<char>>; }
namespace Cookie::Render	 { class Renderer; }
namespace Cookie::Resources  { class ResourcesManager; }
namespace Cookie::Core::Math { union Vec4; }


namespace Cookie::UIwidget
{
	// Allows to create uni-colored textures.
	class TextureEditor final : public WItemBase
	{
		Cookie::Resources::ResourcesManager& resources;

		struct {
			bool creating = false;

			std::string name;
			bool invalidName = false;

			Cookie::Core::Math::Vec4 color = { 0, 0, 0, 1 };

		} newTexture;

	public:
		inline TextureEditor(Cookie::Resources::ResourcesManager& _resources)
			: WItemBase	("Texture editor"),
			  resources	(_resources)
		{}

		void WindowDisplay() override;
	};
}

#endif