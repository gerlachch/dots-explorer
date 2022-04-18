#include <widgets/PropertyEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>

PropertyEdit::PropertyEdit(dots::type::Struct& instance, const dots::type::PropertyPath& propertyPath) :
    m_property{ instance, propertyPath },
    m_header{ fmt::format("{: >{}}{: >2}: {}", "", 2 * (propertyPath.elements().size() - 1), m_property.descriptor().tag(), m_property.descriptor().name()) },
    m_label{ fmt::format("##{}", m_property.descriptor().name()) }
{
    std::string value = dots::to_string(m_property);
    m_buffer.assign(std::max(value.size(), size_t{ 256 }), '\0');
    std::copy(value.begin(), value.end(), m_buffer.begin());
}

std::optional<bool> PropertyEdit::inputParsable() const
{
    return m_inputParsable;
}

void PropertyEdit::render()
{
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(m_header.data());

    ImGui::TableNextColumn();
    if (m_property.descriptor().valueDescriptor().type() != dots::type::Type::Struct)
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);

        if (ImGui::InputText(m_label.data(), m_buffer.data(), m_buffer.size()))
        {
            try
            {
                std::string bufferNullTerminated = m_buffer.data();
                dots::from_string(m_buffer.data(), m_property);
                m_inputParsable = true;
            }
            catch (...)
            {
                m_inputParsable = false;
            }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if (m_inputParsable != std::nullopt)
        {
            if (*m_inputParsable)
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
}
