#include "gui/components/components.hpp"
#include "pointers.hpp"

namespace big
{
	bool components::header_button(const std::string_view text, float xPos, float yPos, const std::string tooltip, float width, float height)
	{
		// Set the button's position
		ImVec2 buttonPosition = ImVec2(xPos * g.window.gui_scale, yPos * g.window.gui_scale); // Custom position
		ImGui::SetCursorPos(buttonPosition);        // Set cursor position

		// Calculate default width and height based on text and scaling
		float textWidth  = ImGui::CalcTextSize(text.data()).x;
		float textHeight = ImGui::CalcTextSize(text.data()).y;

		// Add padding and item spacing to the width and height
		float paddingX = ImGui::GetStyle().FramePadding.x * 0.5 + ImGui::GetStyle().ItemSpacing.x;
		float paddingY = ImGui::GetStyle().FramePadding.y * 0.5 + ImGui::GetStyle().ItemSpacing.y;

		// Default width and height based on text size and padding
		float defaultWidth  = textWidth + paddingX;
		float defaultHeight = textHeight + paddingY;

		// Use the provided width and height if they're not set to the default value (-1.f)
		float buttonWidth  = (width >= 0.f) ? width : defaultWidth;
		float buttonHeight = (height >= 0.f) ? height : defaultHeight;

		// Get the available content region (remaining space)
		ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

		// Constrain button size within the content region (don't overflow)
		buttonWidth  = (buttonWidth > contentRegionAvail.x) ? contentRegionAvail.x : buttonWidth;
		buttonHeight = (buttonHeight > contentRegionAvail.y) ? contentRegionAvail.y : buttonHeight;

		// Push custom style settings for the button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.46f));       // Default color
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.6f)); // Hovered color
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.4f));  // Active color
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.5f, 0.5f});            // Align text within button
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0.f, 0.f});                // Padding around text
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 0.f});                 // Item spacing
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {0.f, 0.f});            // Inner spacing

		// Create the button with calculated or provided width and height
		bool result = ImGui::Button(text.data(), {buttonWidth, buttonHeight});

		// Pop the style settings after the button is created
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);

		// Show tooltip if provided and button is hovered
		if (!tooltip.empty() && ImGui::IsItemHovered())
		{
			ImGui::BeginItemTooltip();
			ImGui::Text(tooltip.c_str()); // Display the custom tooltip text
			ImGui::EndTooltip();
		}

		return result;
	}
	bool components::nav_button(const std::string_view text)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.46f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.4f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.f, 0.2f});
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {5, 5});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 2});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {0, 3});
		bool result = ImGui::Button(text.data(), {((float)*g_pointers->m_gta.m_resolution_x * 0.15f), 0});
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
		
		return result;
	}
}