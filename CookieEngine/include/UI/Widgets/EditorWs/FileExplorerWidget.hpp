#ifndef	__FILE_EXPLORER_W_HPP__
#define __FILE_EXPLORER_W_HPP__

#include "UIwidgetBases.hpp"

namespace std				{ using string = basic_string<char, char_traits<char>, allocator<char>>; }
namespace std::filesystem	{ class path; }
namespace Cookie::Resources { class ResourcesManager; class Texture; }
namespace Cookie			{ class Game; }


namespace Cookie::UIwidget
{
	/*
		The file explorer displays all assets, with folder access as well.
		The save files can be double-clicked on to load them.

		It, of course, has a research bar.
	*/
	class FileExplorer final : public WItemBase
	{
		Cookie::Game& game;

		const Cookie::Resources::Texture* const saveIcon;

	private:
		void ExploreFiles(const std::filesystem::path& path, const char* researchQuery)const;
		bool HasReleventFile(const std::filesystem::path& folderPath, const std::string& researchQuery)const;

	public:
		FileExplorer(Cookie::Game& _game);

		void WindowDisplay() override;
	};
}

#endif