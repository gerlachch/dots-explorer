#include <widgets/dialogs/HostSettingsEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>

HostSettingsEdit::HostSettingsEdit(HostSettings& settings, Host copyHost, Host* editHost) :
    m_popupId{ fmt::format("HostSettingsEdit-{}_Popup", ++M_id) },
    m_settings(settings),
    m_host{ std::move(copyHost) },
    m_endpointEdit{ m_host, false },
    m_editHost(editHost)
{
    if (m_editHost == nullptr)
        m_headerText = "Add Host Info";
    else
        m_headerText = "Edit Host Info";

    m_descriptionBuffer.assign(std::max(m_host.description->size(), size_t{ 1024 }), '\0');
    std::copy(m_host.description->begin(), m_host.description->end(), m_descriptionBuffer.begin());

    ImGui::OpenPopup(m_popupId.data());
}

bool HostSettingsEdit::render()
{
    ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, 0, ImVec2{ 0.5f, 0.5f });

    if (ImGui::BeginPopup(m_popupId.data(), ImGuiWindowFlags_NoMove))
    {
        // header
        {
            ImGui::TextUnformatted(m_headerText);
            ImGui::Separator();
        }

        // input fields
        {
            if (ImGui::BeginTable("HostEditTable", 2))
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Endpoint");

                ImGui::TableNextColumn();
                ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.35f);
                ImGui::AlignTextToFramePadding();
                m_endpointEdit.render();
                ImGui::PopItemWidth();

                ImGui::SameLine();
                if (ImGui::Button("File"))
                    m_fileOpenDialog.emplace();

                if (m_fileOpenDialog && !m_fileOpenDialog->render())
                {
                    if (m_fileOpenDialog->file())
                    {
                        std::filesystem::path path = canonical(*m_fileOpenDialog->file());
                        m_host.endpoint = fmt::format("file:{}{}", path.root_name() == "/" ? "" : "/", path.string());
                        m_host.description = path.filename().string();

                        m_endpointEdit = HostEndpointEdit{ m_host, false };
                        std::string description = *m_host.description;
                        description += '\0';
                        std::copy(description.begin(), description.end(), m_descriptionBuffer.begin());
                    }

                    m_fileOpenDialog.reset();
                }

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Description");

                ImGui::TableNextColumn();
                ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.35f);
                ImGui::AlignTextToFramePadding();
                if (ImGui::InputText("##descriptionInput", m_descriptionBuffer.data(), m_descriptionBuffer.size()))
                {
                    m_host.description = m_descriptionBuffer.data();
                }
                ImGui::PopItemWidth();
                
                ImGui::EndTable();
            }
        }

        // buttons
        {
            bool hasEndpoint = !m_host.endpoint->empty();
            bool hasDescription = m_descriptionBuffer.front() != '\0';
            const char* label = "Save";

            if (hasEndpoint && hasDescription)
            {
                if (ImGui::Button(label))
                {
                    if (m_editHost == nullptr)
                        m_settings.hosts->emplace_back(m_host);
                    else
                        *m_editHost = m_host;

                    m_settings.selectedHost = NoHostSelected;

                    ImGui::CloseCurrentPopup();
                }
            }
            else
            {
                ImGui::BeginDisabled();
                ImGui::Button(label);
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
                ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
        return true;
    }
    else
        return false;
}
