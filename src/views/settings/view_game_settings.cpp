#include "renderer/renderer.hpp"
#include "views/view.hpp"

namespace big
{
	char custom_respawn_text[128] = "WASTED";
	void view::game_settings()
	{
		components::sub_title("Wasted Screen Text"_T.data());
		ImGui::SetNextItemWidth(400);

		bool text_updated = ImGui::InputTextWithHint("##wastedtextinput", "Enter text to display upon death...", custom_respawn_text, IM_ARRAYSIZE(custom_respawn_text), ImGuiInputTextFlags_EnterReturnsTrue);
	}
}
