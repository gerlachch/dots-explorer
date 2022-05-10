#include <widgets/HelpDialog.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/Version.h>

HelpDialog::HelpDialog() :
    m_popupId{ fmt::format("HelpDialog-{}_Popup", ++M_id) }
{
    ImGui::OpenPopup(m_popupId.data());
}

bool HelpDialog::render()
{
    ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, 0, ImVec2{ 0.5f, 0.5f });

    if (ImGui::BeginPopup(m_popupId.data(), ImGuiWindowFlags_NoMove))
    {
        auto render_row = [&](std::string_view column1, std::string_view column2)
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(column1.data(), column1.data() + column1.size());

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(column2.data(), column2.data() + column2.size());
        };

        // render 'Help' section
        {
            ImGui::TextUnformatted("Help");
            ImGui::Separator();


            ImGui::TextUnformatted("General Hints:");
            {
                ImGui::Bullet();
                ImGui::TextUnformatted("Many elements provide a context menu accessible by right-click.");

                ImGui::Bullet();
                ImGui::TextUnformatted("Many tables are sortable by left-clicking the header.");

                ImGui::Bullet();
                ImGui::TextUnformatted("Many tables can be configured by right-clicking a header.");

                ImGui::Bullet();
                ImGui::TextUnformatted("Some table columns can be reordered by dragging a header.");
            }

            ImGui::NewLine();

            ImGui::TextUnformatted("Key List:");
            if (ImGui::BeginTable("KeyTable", 2))
            {
                render_row("TAB           :", "Toggle between cache and trace view.");
                render_row("CTRL+F        :", "Jump to filter input in cache and trace view.");
                render_row("ALT           :", "Show quick view tooltip when hovering over certain items.");
                render_row("ALT+Left-Click:", "Open edit dialog when hovering over certain items.");
                render_row("CTRL          :", "Unlock certain actions (e.g. to remove objects or discard events).");

                ImGui::EndTable();
            }

            ImGui::NewLine();
        }

        // render 'About' section
        {
            ImGui::TextUnformatted("About");
            ImGui::Separator();

            if (ImGui::BeginTable("AboutTable", 2))
            {
                render_row("Installed      :", fmt::format(" {}", Version::CurrentVersion));

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Source (GitHub):");

                ImGui::TableNextColumn();

                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

                constexpr char GitHubUrl[] = "https://github.com/gerlachch/dots-explorer/releases";

                if (ImGui::Button(GitHubUrl))
                {
                    #if defined(_WIN32)
                    std::system(fmt::format("cmd /c start {}", GitHubUrl).data());
                    #elif defined(__linux__)
                    std::system(fmt::format("xdg-open {}", GitHubUrl).data());
                    #endif
                }

                ImGui::PopStyleColor(2);

                ImGui::EndTable();
            }

            ImGui::NewLine();
        }

        if (ImGui::Button("Close") || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
        return true;
    }
    else
    {
        return false;
    }
}
