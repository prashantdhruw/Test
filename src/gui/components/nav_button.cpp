#include "gui/components/components.hpp"
#include "pointers.hpp"

namespace big
{
	bool components::nav_button(const std::string_view text)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.f, 0.2f});
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {5, 5});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 2});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {0, 3});
		bool result = ImGui::Button(text.data(), {((float)*g_pointers->m_gta.m_resolution_x * 0.15f), 0});
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
		if (text == "Unload" && ImGui::IsItemHovered())
		{
			ImGui::BeginItemTooltip();
			ImGui::Text("Unload Chronix from GTA5 (May cause Instability/Crashing)");
			ImGui::EndTooltip();
		}
		return result;
	}
}