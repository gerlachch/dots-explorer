#include <widgets/InstanceEdit.h>
#include <imgui.h>
#include <fmt/format.h>

InstanceEdit::InstanceEdit(dots::type::AnyStruct instance) :
    m_popupId{ fmt::format("InstanceEdit-{}_Popup", ++M_id) },
    m_instance{ std::move(instance) }
{
    for (const dots::type::PropertyPath& propertyPath : m_instance->_descriptor().propertyPaths())
    {
        const dots::type::ProxyProperty<>& property = m_properties.emplace_back(m_instance, propertyPath);
        m_headers.emplace_back(fmt::format("{: >{}}{: >2}: {}", "", 2 * (propertyPath.elements().size() - 1), propertyPath.destination().tag(), propertyPath.destination().name()));
        std::string value = dots::to_string(property);
        std::string& buffer = m_buffers.emplace_back(128, '\0');
        std::copy(value.begin(), value.end(), buffer.begin());
        m_labels.emplace_back(fmt::format("##{}", propertyPath.destination().name()));
        m_inputParsable.emplace_back(std::nullopt);
    }

    ImGui::OpenPopup(m_popupId.data());
}

bool InstanceEdit::render()
{
    ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, 0, ImVec2{ 0.5f, 0.5f });

    if (ImGui::BeginPopup(m_popupId.data(), ImGuiWindowFlags_NoMove))
    {
        // header
        {
            ImGui::TextColored(ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f }, "struct");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f }, "%s", m_instance->_descriptor().name().data());
            ImGui::Separator();
        }

        // properties
        if (ImGui::BeginTable("InstanceEditTable", 2))
        {
            size_t i = 0;

            for (dots::type::ProxyProperty<>& property : m_properties)
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(m_headers[i].data());

                ImGui::TableNextColumn();
                if (property.descriptor().valueDescriptor().type() != dots::type::Type::Struct)
                {
                    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
                    std::string& buffer = m_buffers[i];

                    if (ImGui::InputText(m_labels[i].data(), buffer.data(), buffer.size()))
                    {
                        try
                        {
                            std::string bufferNullTerminated = buffer.data();
                            dots::from_string(buffer.data(), property);
                            m_inputParsable[i] = true;
                        }
                        catch (...)
                        {
                            m_inputParsable[i] = false;
                        }
                    }
                    ImGui::PopItemWidth();

                    ImGui::SameLine();
                    if (std::optional<bool> parsable = m_inputParsable[i]; parsable != std::nullopt)
                    {
                        if (*parsable)
                        {
                            ImGui::TextColored(ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f }, "Ok   ");
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "Error");
                        }
                    }
                    else
                    {
                        ImGui::TextUnformatted("     ");
                    }
                }

                ++i;
            }
            
            ImGui::EndTable();
        }

        // buttons
        {
            bool allInputParsable = std::none_of(m_inputParsable.begin(), m_inputParsable.end(), [](std::optional<bool> b){ return b == false; });

            if (allInputParsable)
            {
                if (ImGui::Button("Publish"))
                {
                    dots::publish(m_instance);
                    ImGui::CloseCurrentPopup();
                }
            }
            else
            {
                ImGui::BeginDisabled();
                ImGui::Button("Publish");
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
