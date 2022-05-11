#include <widgets/HostSettingsEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

HostSettingsEdit::HostSettingsEdit(HostSettings& settings, Host* editHost/* = nullptr*/) :
    m_popupId{ fmt::format("HostSettingsEdit-{}_Popup", ++M_id) },
    m_endpointBuffer(256, '\0'),
    m_descriptionBuffer(256, '\0'),
    m_settings(settings),
    m_editHost(editHost)
{
    if (m_editHost == nullptr)
    {
        m_headerText = "Add Host Info";
    }
    else
    {
        m_headerText = "Edit Host Info";
        std::copy(m_editHost->endpoint->begin(), m_editHost->endpoint->end(), m_endpointBuffer.begin());
        std::copy(m_editHost->description->begin(), m_editHost->description->end(), m_descriptionBuffer.begin());
    }

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
                ImGui::InputText("##endpointInput", m_endpointBuffer.data(), m_endpointBuffer.size());
                ImGui::PopItemWidth();

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Description");

                ImGui::TableNextColumn();
                ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.35f);
                ImGui::AlignTextToFramePadding();
                ImGui::InputText("##descriptionInput", m_descriptionBuffer.data(), m_descriptionBuffer.size());
                ImGui::PopItemWidth();
                
                ImGui::EndTable();
            }
        }

        // buttons
        {
            bool hasEndpoint = m_endpointBuffer.front() != '\0';
            bool hasDescription = m_endpointBuffer.front() != '\0';
            const char* label = "Save";

            if (hasEndpoint && hasDescription)
            {
                if (ImGui::Button(label))
                {
                    Host host{
                        Host::endpoint_i{ m_endpointBuffer.data() },
                        Host::description_i{ m_descriptionBuffer.data() }
                    };

                    if (m_editHost == nullptr)
                    {
                        m_settings.hosts->emplace_back(std::move(host));
                    }
                    else
                    {
                        *m_editHost = host;
                    }

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
            {
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
        return true;
    }
    else
    {
        return false;
    }
}
