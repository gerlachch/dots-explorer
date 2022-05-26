#include <widgets/dialogs/FilterSettingsEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>

FilterSettingsEdit::FilterSettingsEdit(FilterSettings& settings, Filter* editFilter/* = nullptr*/) :
    m_popupId{ fmt::format("FilterSettingsEdit-{}_Popup", ++M_id) },
    m_filterExpressionEdit{ {}, {} },
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
        m_filterExpressionEdit = std::string_view{ *m_editFilter->expression };
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
                m_filterExpressionEdit.render();
                ImGui::PopItemWidth();
                ImGuiExt::TooltipLastHoveredItem("Types can be filtered by specifying substrings or ECMAScript regular expressions.");

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
            bool hasValidExpression = m_filterExpressionEdit.isValid();
            bool hasDescription = m_descriptionBuffer.front() != '\0';
            const char* label = "Save";

            if (hasValidExpression && hasDescription)
            {
                if (ImGui::Button(label))
                {
                    Filter filter{
                        Filter::expression_i{ m_filterExpressionEdit.text().first },
                        Filter::description_i{ m_descriptionBuffer.data() }
                    };

                    if (m_editFilter == nullptr)
                    {
                        m_settings.storedFilters->emplace_back(std::move(filter));
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
