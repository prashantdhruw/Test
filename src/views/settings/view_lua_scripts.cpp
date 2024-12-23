#include "fiber_pool.hpp"
#include "lua/lua_manager.hpp"
#include "views/view.hpp"

#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp> // Include nlohmann/json for JSON parsing
#include <string>
#include <vector>

namespace big
{
	static std::weak_ptr<lua_module> selected_module{};

	struct Repository
	{
		std::string name;
		std::string html_url;
		std::string contents_url;
	};

	struct File
	{
		std::string name;
		std::string download_url;
	};

	class GitHubRepositories
	{
	public:
		static std::vector<Repository> FetchRepositories()
		{
			CURL* curl = curl_easy_init();
			if (!curl)
			{
				LOG(FATAL) << "CURL initialization failed!" << std::endl;
				return {};
			}

			std::string url = "https://api.github.com/orgs/YimMenu-Lua/repos?type=all";
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			struct curl_slist* headers = nullptr;
			headers                    = curl_slist_append(headers, "User-Agent: request");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			std::string response_data;
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

			CURLcode res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			if (res != CURLE_OK)
			{
				LOG(FATAL) << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
				return {};
			}

			try
			{
				nlohmann::json root = nlohmann::json::parse(response_data);

				std::vector<Repository> repositories;
				for (const auto& repo : root)
				{
					Repository repository;
					repository.name         = repo["name"];
					repository.html_url     = repo["html_url"];
					repository.contents_url = repo["contents_url"];
					repositories.push_back(repository);
				}

				return repositories;
			}
			catch (const nlohmann::json::exception& e)
			{
				LOG(FATAL) << "Failed to parse JSON response: " << e.what() << std::endl;
				return {};
			}
		}

		static std::vector<File> FetchRepositoryContents(const std::string& contents_url, const std::string& relative_path = "")
		{
			CURL* curl = curl_easy_init();
			if (!curl)
			{
				LOG(FATAL) << "CURL initialization failed!" << std::endl;
				return {};
			}

			std::string url = contents_url;
			size_t pos      = url.find("{+path}");
			if (pos != std::string::npos)
			{
				url.replace(pos, 7, ""); // Remove {+path}
			}

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			struct curl_slist* headers = nullptr;
			headers                    = curl_slist_append(headers, "User-Agent: request");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			std::string response_data;
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

			CURLcode res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			if (res != CURLE_OK)
			{
				LOG(FATAL) << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
				return {};
			}

			try
			{
				nlohmann::json root = nlohmann::json::parse(response_data);

				std::vector<File> lua_files;
				for (const auto& item : root)
				{
					if (item["type"] == "file" && item["name"].get<std::string>().ends_with(".lua"))
					{
						File f;
						f.name =
						    relative_path.empty() ? item["name"].get<std::string>() : relative_path + "/" + item["name"].get<std::string>();
						f.download_url = item["download_url"];
						lua_files.push_back(f);
					}
					else if (item["type"] == "dir")
					{
						// Recursive call for subfolder
						std::string subfolder_url = item["url"];
						std::string new_relative_path =
						    relative_path.empty() ? item["name"].get<std::string>() : relative_path + "/" + item["name"].get<std::string>();

						auto subfolder_files = FetchRepositoryContents(subfolder_url, new_relative_path);
						lua_files.insert(lua_files.end(), subfolder_files.begin(), subfolder_files.end());
					}
				}

				return lua_files;
			}
			catch (const nlohmann::json::exception& e)
			{
				LOG(FATAL) << "Failed to parse JSON response: " << e.what() << std::endl;
				return {};
			}
		}

	private:
		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
		{
			size_t total_size = size * nmemb;
			((std::string*)userp)->append((char*)contents, total_size);
			return total_size;
		}
	};

	void DownloadScript(const std::string& download_url, const std::string& relative_path)
	{
		const char* appdata = std::getenv("APPDATA");
		if (appdata == nullptr)
		{
			LOG(FATAL) << "Error: %APPDATA% environment variable not found!" << std::endl;
			return;
		}

		std::filesystem::path output_path = std::filesystem::path(appdata) / "Chronix" / "scripts" / relative_path;

		std::filesystem::create_directories(output_path.parent_path());

		FILE* file = fopen(output_path.string().c_str(), "wb");
		if (!file)
		{
			LOG(FATAL) << "Failed to open file for writing: " << output_path << std::endl;
			return;
		}

		CURL* curl = curl_easy_init();
		if (!curl)
		{
			LOG(FATAL) << "CURL initialization failed!" << std::endl;
			fclose(file);
			return;
		}

		curl_easy_setopt(curl, CURLOPT_URL, download_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			LOG(FATAL) << "CURL download failed: " << curl_easy_strerror(res) << std::endl;
		}

		fclose(file);
		curl_easy_cleanup(curl);
		LOG(INFO) << "Download completed: " << output_path << std::endl;
	}

	void view::lua_scripts()
	{
		ImGui::PushItemWidth(250);
		components::sub_title("VIEW_LUA_SCRIPTS_LOADED_LUA_SCRIPTS"_T);

		if (components::button("VIEW_LUA_SCRIPTS_RELOAD_ALL"_T))
		{
			g_fiber_pool->queue_job([] {
				g_lua_manager->trigger_event<menu_event::ScriptsReloaded>();
				g_lua_manager->unload_all_modules();
				g_lua_manager->load_all_modules();
			});
		}
		ImGui::SameLine();
		ImGui::Checkbox("VIEW_LUA_SCRIPTS_AUTO_RELOAD_CHANGED_SCRIPTS"_T.data(), &g.lua.enable_auto_reload_changed_scripts);

		if (components::button("VIEW_LUA_SCRIPTS_OPEN_LUA_SCRIPTS_FOLDER"_T))
		{
			std::string command = "explorer.exe /select," + g_lua_manager->get_scripts_folder().get_path().string();
			std::system(command.c_str());
		}

		ImGui::BeginGroup();
		components::sub_title("ENABLED_LUA_SCRIPTS"_T);
		{
			if (ImGui::BeginListBox("##empty", ImVec2(200, 200)))
			{
				g_lua_manager->for_each_module([](auto& module) {
					if (ImGui::Selectable(module->module_name().c_str(),
					        !selected_module.expired() && selected_module.lock().get() == module.get()))
					{
						selected_module = module;
					}
				});

				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		components::sub_title("DISABLED_LUA_SCRIPTS"_T);
		{
			if (ImGui::BeginListBox("##disabled_empty", ImVec2(200, 200)))
			{
				g_lua_manager->for_each_disabled_module([](auto& module) {
					if (ImGui::Selectable(module->module_name().c_str(),
					        !selected_module.expired() && selected_module.lock().get() == module.get()))
					{
						selected_module = module;
					}
				});

				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		if (!selected_module.expired())
		{
			ImGui::Separator();

			ImGui::Text(std::format("{}: {}",
			    "VIEW_LUA_SCRIPTS_SCRIPTS_REGISTERED"_T,
			    selected_module.lock()->m_registered_scripts.size())
			                .c_str());
			ImGui::Text(std::format("{}: {}",
			    "VIEW_LUA_SCRIPTS_MEMORY_PATCHES_REGISTERED"_T,
			    selected_module.lock()->m_registered_patches.size())
			                .c_str());
			ImGui::Text(std::format("{}: {}",
			    "VIEW_LUA_SCRIPTS_SCRIPT_PATCHES_REGISTERED"_T,
			    selected_module.lock()->m_registered_script_patches.size())
			                .c_str());
			ImGui::Text(
			    std::format("{}: {}", "VIEW_LUA_SCRIPTS_GUI_TABS_REGISTERED"_T, selected_module.lock()->m_gui.size()).c_str());

			const auto id = selected_module.lock()->module_id();
			if (components::button("VIEW_LUA_SCRIPTS_RELOAD"_T))
			{
				const std::filesystem::path module_path = selected_module.lock()->module_path();

				g_lua_manager->unload_module(id);
				selected_module = g_lua_manager->load_module(module_path);
			}

			const auto is_disabled = selected_module.lock()->is_disabled();
			if (!is_disabled && components::button<ImVec2{0, 0}, ImVec4{0.58f, 0.15f, 0.15f, 1.f}>("DISABLE"_T))
			{
				selected_module = g_lua_manager->disable_module(id);
			}
			else if (is_disabled && components::button("ENABLE"_T.data()))
			{
				selected_module = g_lua_manager->enable_module(id);
			}
		}

		ImGui::EndGroup();

		if (components::button<ImVec2{0, 0}, ImVec4{0.58f, 0.15f, 0.15f, 1.f}>("DISABLE_ALL_LUA_SCRIPTS"_T))
		{
			g_lua_manager->disable_all_modules();
		}
		ImGui::SameLine();
		if (components::button("ENABLE_ALL_LUA_SCRIPTS"_T))
		{
			g_lua_manager->enable_all_modules();
		}

		static std::vector<Repository> repositories;
		static bool repositories_fetched = false;
		static std::vector<File> repository_contents;
		static bool repository_selected = false;

		if (components::button("FETCH_LUA_SCRIPTS_REPOSITORIES"_T))
		{
			repositories         = GitHubRepositories::FetchRepositories();
			repositories_fetched = true;
		}
		ImGui::BeginGroup();
		if (repositories_fetched)
		{
			components::sub_title("LUA_SCRIPTS_REPOSITORIES"_T);
			if (ImGui::BeginListBox("##repositories", ImVec2(200, 200)))
			{
				for (const auto& repo : repositories)
				{
					if (ImGui::Selectable(repo.name.c_str()))
					{
						repository_contents = GitHubRepositories::FetchRepositoryContents(repo.contents_url);
						repository_selected = true;
					}
				}
				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		if (repository_selected)
		{
			components::sub_title("REPOSITORY_CONTENTS"_T);
			if (ImGui::BeginListBox("##repository_contents", ImVec2(200, 200)))
			{
				for (const auto& file : repository_contents)
				{
					if (ImGui::Selectable(file.name.c_str()))
					{
						DownloadScript(file.download_url, file.name);
					}
				}
				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();
	}
}
