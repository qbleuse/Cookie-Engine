#include <string>
#include <filesystem>
#include "Game.hpp"
#include "FileExplorerWidget.hpp"

#include <imgui.h>
#include "Serialization.hpp"

#include "CustomImWidget.hpp"

namespace fs = std::filesystem;
using namespace ImGui;
using namespace Cookie::UIwidget;


FileExplorer::FileExplorer(Cookie::Game& _game)
            : WItemBase     ("File explorer"),
              game          (_game),
              saveIcon      (_game.resources.icons["Assets/EditorUIcons/Save2.ico"].get())
{}


void FileExplorer::ExploreFiles(const fs::path& path, const char* researchQuery)const
{
    if (fs::exists(path) && fs::is_directory(path))
    {
        for (const fs::directory_entry& entry : fs::directory_iterator(path))
        {
            const fs::path& filename = entry.path().c_str();
            if (fs::is_directory(entry.status()) && HasReleventFile(entry, researchQuery))
            {
                if (TreeNode(filename.filename().string().c_str()))
                {
                    ExploreFiles(entry, researchQuery);
                    TreePop();
                }
            }
        }

        bool prevWasSave = false;
        for (const fs::directory_entry& entry : fs::directory_iterator(path))
        {
            const fs::path& filename = entry.path().c_str();
            if (fs::is_regular_file(entry.status()))
            {
                if (filename.string().find(researchQuery) != std::string::npos)
                {
                    if (filename.extension().string() == ".CAsset")
                    {
                        if (prevWasSave)
                        {
                            (GetContentRegionAvail().x < 70.f) ? NewLine() : SameLine();
                        }

                        if (Custom::FileButton(filename.filename().string().c_str(), saveIcon->GetResourceView()))
                        {
                            Cookie::Resources::Serialization::Load::LoadScene(filename.string().c_str(), game);
                            game.SetScene();
                        }

                        prevWasSave = true;
                    }
                    else
                    {
                        Text("%s", filename.filename().string().c_str());

                        prevWasSave = false;
                    }
                }
            }
        }
    }
}

bool FileExplorer::HasReleventFile(const std::filesystem::path& folderPath, const std::string& researchQuery)const
{
    if (researchQuery.size() == 0) return true;

    for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
    {
        const fs::path& filename = entry.path().c_str();
        if (fs::is_directory(entry.status()))
        {
            if (HasReleventFile(entry, researchQuery)) return true;
        }
        else if (fs::is_regular_file(entry.status()))
        {
            if (filename.string().find(researchQuery) != std::string::npos) return true;
        }
    }

    return false;
}

void FileExplorer::WindowDisplay()
{
    TryBeginWindow(ImGuiWindowFlags_HorizontalScrollbar)
    {
        static char searchQuery[25]{ 0 };
        InputText("File search", &searchQuery[0], 25);


        PushStyleVar(ImGuiStyleVar_IndentSpacing, 35.f);

        ExploreFiles("Assets/", &searchQuery[0]);

        PopStyleVar();
    }

    ImGui::End();
}