#include "components.hpp"
#include "services/gui/gui_service.hpp"
#include "services/translation_service/translation_service.hpp"

namespace big
{
	void components::nav_item(std::pair<tabs, navigation_struct>& navItem, int nested)
	{
		const bool current_tab = !g_gui_service->get_selected_tab().empty() && g_gui_service->get_selected_tab().size() > nested
		    && navItem.first == g_gui_service->get_selected_tab().at(nested);


		if (current_tab)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.29f, 0.f, 0.f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.29f, 0.45f, 0.69f, 1.f));
		}

		const char* key = nullptr;
		if (key = g_translation_service.get_translation(navItem.second.name).data(); !key)
		{
			key = navItem.second.name;
		}
		if (components::nav_button(key))
		{
			g_gui_service->set_selected(navItem.first);
		}

		if (current_tab)
		{
			ImGui::PopStyleColor(2);
		}

		if (current_tab && !navItem.second.sub_nav.empty())
		{
			ImDrawList* draw_list = ImGui::GetForegroundDrawList();

			for (std::pair<tabs, navigation_struct> item : navItem.second.sub_nav)
			{
				nav_item(item, nested + 1);
			}
		}

		g_gui_service->increment_nav_size();
	}
}
