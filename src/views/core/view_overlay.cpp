#include "gta/pools.hpp"
#include "gta_util.hpp"
#include "gui.hpp"
#include "pointers.hpp"
#include "views/view.hpp"

namespace big
{
	void view::overlay()
	{
		if (!g.window.ingame_overlay.opened || (g_gui->is_open() && !g.window.ingame_overlay.show_with_menu_opened))
			return;

		g_gui->push_theme_colors();

		//ImGui::SetNextWindowPos(ImVec2({(float)*g_pointers->m_gta.m_resolution_x / 300.f, (float)*g_pointers->m_gta.m_resolution_y - 30.f}), ImGuiWindowFlags_None, ImVec2(0.0f, 0.0f));
		//ImGui::SetNextWindowSize({1200.f, 50.f});
		ImGuiIO& io        = ImGui::GetIO();
		float windowWidth  = io.DisplaySize.x * 0.595f; // Adjust this percentage as needed (e.g., 63% width)
		float windowHeight = 91.f;     // Keep the height fixed or adjust similarly if needed
		float centerX      = (io.DisplaySize.x - windowWidth) / 2.0f;

		ImGui::SetNextWindowPos(ImVec2(centerX, (float)*g_pointers->m_gta.m_resolution_y - 100.f), ImGuiCond_Always);
		
		ImGui::SetNextWindowSize({windowWidth, windowHeight});
		ImGui::SetNextWindowBgAlpha(0.3f);

		auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		if (!g_gui->is_open())
		{
			window_flags |= ImGuiWindowFlags_NoMouseInputs;
		}
		if (ImGui::Begin("overlay", nullptr, window_flags))
		{
			ImGuiIO& io           = ImGui::GetIO();
			float baseScaleFactor = 1.0f; // Your desired base scale factor

			// Calculate a proportional scale factor based on the window width
			float scaleFactor = io.DisplaySize.x / 1920.0f; // Assuming 1920 is your base resolution
			ImGui::SetWindowFontScale(baseScaleFactor * scaleFactor);
			if (g.window.ingame_overlay.show_fps)
			{
				ImGui::Text(std::format("{:.0f} {}", ImGui::GetIO().Framerate, "VIEW_OVERLAY_FPS"_T).c_str());
			}
			ImGui::SameLine();
			if (CNetworkPlayerMgr* network_player_mgr = gta_util::get_network_player_mgr(); g.window.ingame_overlay.show_players)
			{
				ImGui::Text(std::format("{}: {}/{}", "PLAYERS"_T, network_player_mgr->m_player_count, network_player_mgr->m_player_limit).c_str());
			}
			ImGui::SameLine();
			if (g.window.ingame_overlay.show_game_versions)
			{
				ImGui::Text(
				    std::format("{}: {}", "VIEW_OVERLAY_GAME_VERSION"_T, g_pointers->m_gta.m_game_version).c_str());
				ImGui::SameLine();
				ImGui::Text(
				    std::format("{}: {}", "VIEW_OVERLAY_ONLINE_VERSION"_T, g_pointers->m_gta.m_online_version).c_str());
			}
			if (g.window.ingame_overlay.show_indicators)
			{
				if (g.window.ingame_overlay_indicators.show_player_godmode)
					components::overlay_indicator("VIEW_OVERLAY_PLAYER_GODMODE"_T, g.self.god_mode);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_off_radar)
					components::overlay_indicator("OFF_THE_RADAR"_T, g.self.off_radar);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_vehicle_godmode)
					components::overlay_indicator("VIEW_OVERLAY_VEHICLE_GODMODE"_T, g.vehicle.god_mode);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_never_wanted)
					components::overlay_indicator("NEVER_WANTED"_T, g.self.never_wanted);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_infinite_ammo)
					components::overlay_indicator("VIEW_OVERLAY_INFINITE_AMMO"_T, g.weapons.infinite_ammo);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_always_full_ammo)
					components::overlay_indicator("VIEW_OVERLAY_ALWAYS_FULL_AMMO"_T, g.weapons.always_full_ammo);

				if (g.window.ingame_overlay_indicators.show_infinite_mag)
					components::overlay_indicator("VIEW_OVERLAY_INFINITE_MAGAZINE"_T, g.weapons.infinite_mag);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_aimbot)
					components::overlay_indicator("VIEW_OVERLAY_AIMBOT"_T, g.weapons.aimbot.enable);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_triggerbot)
					components::overlay_indicator("VIEW_OVERLAY_TRIGGERBOT"_T, g.weapons.triggerbot);
				ImGui::SameLine();
				if (g.window.ingame_overlay_indicators.show_invisibility)
					components::overlay_indicator("INVISIBILITY"_T, g.self.invisibility);
			}

			if (g.window.ingame_overlay.show_position && g_local_player)
			{
				auto& pos = *g_local_player->get_position();

				ImGui::TextWrapped(std::format("{}: {:.2f}, {:.2f}, {:.2f}", "VIEW_OVERLAY_POSITION"_T, pos.x, pos.y, pos.z).c_str());
			}

			if (g.window.ingame_overlay.show_replay_interface)
			{
				if (*g_pointers->m_gta.m_ped_pool || (*g_pointers->m_gta.m_vehicle_pool && **g_pointers->m_gta.m_vehicle_pool)
				    || *g_pointers->m_gta.m_prop_pool);
					//ImGui::Separator();

				/* if (*g_pointers->m_gta.m_ped_pool)
				{
					ImGui::Text(std::format("{}: {}/{}",
					    "VIEW_OVERLAY_PED_POOL"_T,
					    (*g_pointers->m_gta.m_ped_pool)->get_item_count(),
					    (*g_pointers->m_gta.m_ped_pool)->m_size)
					                .c_str());
				}

				if (*g_pointers->m_gta.m_vehicle_pool && **g_pointers->m_gta.m_vehicle_pool)
				{
					ImGui::Text(std::format("{}: {}/{}",
					    "VIEW_OVERLAY_VEHICLE_POOL"_T,
					    (**g_pointers->m_gta.m_vehicle_pool)->m_item_count,
					    (**g_pointers->m_gta.m_vehicle_pool)->m_size)
					                .c_str());
				}

				if (*g_pointers->m_gta.m_prop_pool)
				{
					ImGui::Text(std::format("{}: {}/{}",
					    "VIEW_OVERLAY_OBJECT_POOL"_T,
					    (*g_pointers->m_gta.m_prop_pool)->get_item_count(),
					    (*g_pointers->m_gta.m_prop_pool)->m_size)
					                .c_str());
				}*/
			}
		}
		ImGui::End();

		g_gui->pop_theme_colors();
	}
}
