#include "renderer/renderer.hpp"
#include "views/view.hpp"

namespace big
{
	char custom_respawn_text[128] = "WASTED";
	char custom_off_radar_text[128] = "OFF THE RADAR";
	void view::game_settings()
	{
		components::sub_title("Wasted Screen Text"_T.data());
		ImGui::SetNextItemWidth(400);

		bool text_updated = ImGui::InputTextWithHint("##wastedtextinput", "Enter text to display upon death...", custom_respawn_text, IM_ARRAYSIZE(custom_respawn_text), ImGuiInputTextFlags_EnterReturnsTrue);
		if (ImGui::IsItemActive())
		{
			g.self.hud.typing = TYPING_TICKS;
		}
		components::sub_title("Off Radar Text"_T.data());
		ImGui::SetNextItemWidth(400);

		bool otr_text_updated = ImGui::InputTextWithHint("##otrtextinput", "Enter text to display while off the radar...", custom_off_radar_text, IM_ARRAYSIZE(custom_off_radar_text), ImGuiInputTextFlags_EnterReturnsTrue);
		if (ImGui::IsItemActive())
		{
			g.self.hud.typing = TYPING_TICKS;
		}
	}
}
