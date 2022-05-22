#include <widgets/dialogs/HelpDialog.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/Version.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>

HelpDialog::HelpDialog(const GitHubReleaseInfo* releaseInfo/* = nullptr*/) :
    m_popupId{ fmt::format("HelpDialog-{}_Popup", ++M_id) },
    m_releaseInfo(releaseInfo)
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

        auto render_hyperlink_row = [&](std::string_view column1, const std::string& uri)
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(column1.data(), column1.data() + column1.size());

            ImGui::TableNextColumn();
            ImGuiExt::Hyperlink(uri);
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
                if (m_releaseInfo == nullptr)
                {
                    render_row("Installed     :", fmt::format(" {}", Version::CurrentVersion));
                    render_row("Available     :", " <unknown>");
                    render_hyperlink_row("Download      :", fmt::format(" https://github.com/gerlachch/dots-explorer/releases/tag/{}", Version::CurrentVersion));
                }
                else
                {
                    bool updateAvailable = m_releaseInfo->tag_name.isValid() && *m_releaseInfo->tag_name > Version::CurrentVersion;

                    ImGui::PushStyleColor(ImGuiCol_Text, updateAvailable ? ColorThemeActive.Error : ImGui::GetStyle().Colors[ImGuiCol_Text]);
                    render_row("Installed     :", fmt::format(" {}", Version::CurrentVersion));
                    ImGui::PopStyleColor();

                    ImGui::PushStyleColor(ImGuiCol_Text, updateAvailable ? ColorThemeActive.Success : ImGui::GetStyle().Colors[ImGuiCol_Text]);
                    render_row("Available     :", fmt::format(" {}", *m_releaseInfo->tag_name));
                    ImGui::PopStyleColor();

                    render_hyperlink_row("Download      :", *m_releaseInfo->html_url);
                }

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
