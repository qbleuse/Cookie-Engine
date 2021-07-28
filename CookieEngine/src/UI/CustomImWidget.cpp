#include "CustomImWidget.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

using namespace ImGui;


bool Custom::FileButton(const char* filename, ImTextureID image_ID)
{
    BeginGroup();

    ImVec2&& preMovePos = GetCursorPos();
    
    bool selected;
    Selectable("##FILEBUTTON", &selected, ImGuiSelectableFlags_AllowDoubleClick, {70, 70}, true);
    SetCursorPos(preMovePos);

    ImGuiWindow* window = GetCurrentWindow();
    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2{70, 70});

    window->DrawList->AddImage(image_ID, bb.Min + ImVec2(10, 1), bb.Max - ImVec2(10, 20), {0, 0}, {1, 1}, GetColorU32({1, 1, 1, 1}));

    SetCursorPos(preMovePos + ImVec2{0, 50});
    
    TextSnip(filename, 10);

    SetCursorPos(preMovePos);
    ItemSize({70, 70});

    EndGroup();

    return selected && IsMouseDoubleClicked(0);
}

void Custom::Zoomimage(ImTextureID image_ID, float image_w, float image_h, short zoomPower, bool magnify)
{   
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImVec2 uv_min       = ImVec2(0.0f, 0.0f);               // Top-left
    ImVec2 uv_max       = ImVec2(1.0f, 1.0f);               // Lower-right
    ImVec4 tint_col     = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    ImVec4 border_col   = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);   // 50% opaque white
    
    ImGui::Image(image_ID, {image_w, image_h}, uv_min, uv_max, tint_col, border_col);

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        if (magnify)
        {
            float region_sz = (image_w + image_w)/6;
            float region_x = GetIO().MousePos.x - pos.x - region_sz;
            float region_y = GetIO().MousePos.y - pos.y - region_sz;
            float zoom = 15.f;
        
            ImVec2 uv0 = ImVec2((region_x) / image_w, (region_y) / image_h);
            ImVec2 uv1 = ImVec2((region_x + region_sz) / image_w, (region_y + region_sz) / image_h);
        
            ImGui::Image(image_ID, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
        }
        else
        {
            ImGui::Image(image_ID, ImVec2{image_w, image_h} * zoomPower);
        }
        
        ImGui::EndTooltip();
    }
}