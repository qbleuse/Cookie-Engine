#ifndef	__CUSTOM_IMWIDGET_HPP__
#define __CUSTOM_IMWIDGET_HPP__

#include <imgui.h>

// Namespace for custom-made ImGui widegets.
namespace ImGui::Custom
{
	// Displays up to the given number of chars, if exceeded, the rest can be peeked in a tooltip by hovering the text.
	inline void TextSnip(const char* text, short maxChars)
    {
        if (strlen(text) > maxChars)
        {
            Text("%.*s...", maxChars - 3, text);

            if (IsItemHovered())
            {
                BeginTooltip();
                Text("%s", text);
                EndTooltip();
            }
        }
        else
            Text("%s", text);
    }


	// Used to display a button with an image atop the name of a file. Hovering above a long file name displays it in a tooltip.
	// Note: This whole function and each sub-widget within is bundled-up in a group to immensely facilitate alignment and placement.
	bool FileButton(const char* filename, ImTextureID image_ID);


	// Displays an image that can be zoomed-in by hovering it. If magnify is on it will zoom in a part of the image at the cursor with greater precision.
	void Zoomimage(ImTextureID image_ID, float image_w, float image_h, short zoomPower = 15, bool magnify = false);
}

#endif