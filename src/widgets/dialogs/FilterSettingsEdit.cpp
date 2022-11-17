#include <widgets/dialogs/FilterSettingsEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>

FilterSettingsEdit::FilterSettingsEdit(FilterSettings& settings, Filter* existingFilter/* = nullptr*/) :
    m_popupId{ fmt::format("FilterSettingsEdit-{}_Popup", ++M_id) },
    m_descriptionBuffer(256, '\0'),
    m_settings(settings),
    m_existingFilter(existingFilter)
{
    if (m_existingFilter == nullptr)
    {
        m_headerText = "Add Filter";

        for (auto& property : m_filter)
        {
            property.valueOrEmplace();
        }
    }
    else
    {
        m_headerText = "Edit Filter";
        m_filter = *m_existingFilter;
        std::copy(m_existingFilter->description->begin(), m_existingFilter->description->end(), m_descriptionBuffer.begin());
    }

    m_filterExpressionEdit.emplace(m_filter, "");
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
                ImGui::TextUnformatted("Expression");

                ImGui::TableNextColumn();
                ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.35f);
                ImGui::AlignTextToFramePadding();
                m_filterExpressionEdit->render();
                ImGui::PopItemWidth();

                ImGui::SameLine();
                ImGuiExt::ToggleButton("Aa", *m_filter.matchCase, "Match case");

                ImGui::SameLine();
                if (ImGuiExt::ToggleButton("Re", *m_filter.regex, "Interpret expression as a regular expression instead of a quick filter."))
                {
                    m_filterExpressionEdit.emplace(m_filter, "");
                }

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
            bool hasValidExpression = m_filterExpressionEdit->isValid();
            bool hasDescription = m_descriptionBuffer.front() != '\0';
            const char* label = "Save";

            if (hasValidExpression && hasDescription)
            {
                if (ImGui::Button(label))
                {
                    m_filter.description = m_descriptionBuffer.data();

                    if (m_existingFilter == nullptr)
                    {
                        m_settings.storedFilters->emplace_back(std::move(m_filter));
                    }
                    else
                    {
                        *m_existingFilter = m_filter;
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
