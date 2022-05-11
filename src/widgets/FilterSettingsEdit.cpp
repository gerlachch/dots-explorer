#include <widgets/FilterSettingsEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

FilterSettingsEdit::FilterSettingsEdit(FilterSettings& settings, Filter* editFilter/* = nullptr*/) :
    m_popupId{ fmt::format("FilterSettingsEdit-{}_Popup", ++M_id) },
    m_regexBuffer(256, '\0'),
    m_descriptionBuffer(256, '\0'),
    m_settings(settings),
    m_editFilter(editFilter)
{
    if (m_editFilter == nullptr)
    {
        m_headerText = "Add Filter";
    }
    else
    {
        m_headerText = "Edit Filter";
        std::copy(m_editFilter->regex->begin(), m_editFilter->regex->end(), m_regexBuffer.begin());
        std::copy(m_editFilter->description->begin(), m_editFilter->description->end(), m_descriptionBuffer.begin());
    }

    ImGui::OpenPopup(m_popupId.data());
}

bool FilterSettingsEdit::render()
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
            if (ImGui::BeginTable("FilterEditTable", 2))
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Regex");

                ImGui::TableNextColumn();
                ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.35f);
                ImGui::AlignTextToFramePadding();
                ImGui::InputText("##regexInput", m_regexBuffer.data(), m_regexBuffer.size());
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
            bool hasRegex = m_regexBuffer.front() != '\0';
            bool hasDescription = m_regexBuffer.front() != '\0';
            const char* label = "Save";

            if (hasRegex && hasDescription)
            {
                if (ImGui::Button(label))
                {
                    Filter filter{
                        Filter::regex_i{ m_regexBuffer.data() },
                        Filter::description_i{ m_descriptionBuffer.data() }
                    };

                    if (m_editFilter == nullptr)
                    {
                        m_settings.filters->emplace_back(std::move(filter));
                    }
                    else
                    {
                        *m_editFilter = filter;
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
