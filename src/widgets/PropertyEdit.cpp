#include <widgets/PropertyEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>

PropertyEdit::PropertyEdit(dots::type::Struct& instance, const dots::type::PropertyPath& propertyPath) :
    m_property{ instance, propertyPath },
    m_header{ fmt::format("{: >{}}{: >2}: {}", "", 2 * (propertyPath.elements().size() - 1), m_property.descriptor().tag(), m_property.descriptor().name()) },
    m_inputLabel{ fmt::format("##PropertyEdit_{}_Input", m_property.descriptor().name()) },
    m_invalidateLabel{ fmt::format("X##PropertyEdit_{}_Invalidate", m_property.descriptor().name()) }
{
    std::string value = dots::to_string(m_property);
    m_buffer.assign(std::max(value.size(), size_t{ 256 }), '\0');
    std::copy(value.begin(), value.end(), m_buffer.begin());

    switch (m_property.descriptor().valueDescriptor().type())
    {
        case dots::type::Type::boolean:
            m_color = ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f };
            break;
        case dots::type::Type::string:
            m_color = ImVec4{0.91f, 0.79f, 0.73f, 1.0f };
            break;
        case dots::type::Type::int8:
        case dots::type::Type::uint8:
        case dots::type::Type::int16:
        case dots::type::Type::uint16:
        case dots::type::Type::int32:
        case dots::type::Type::uint32:
        case dots::type::Type::int64:
        case dots::type::Type::uint64:
        case dots::type::Type::float32:
        case dots::type::Type::float64:
            m_color = ImVec4{ 0.72f, 0.84f, 0.64f, 1.0f };
            break;
        case dots::type::Type::Enum:
            m_color = ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f };
            break;
        case dots::type::Type::property_set:
        case dots::type::Type::timepoint:
        case dots::type::Type::steady_timepoint:
        case dots::type::Type::duration:
        case dots::type::Type::uuid:
        case dots::type::Type::Vector:
        case dots::type::Type::Struct:
        default:
            m_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
            break;
    }
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
    if (dots::type::Type type = m_property.descriptor().valueDescriptor().type(); type != dots::type::Type::Struct)
    {
        if (m_property.isValid())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, *m_color);
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        }
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);

        if (type == dots::type::Type::boolean)
        {
            constexpr const char* Items[] = { "<invalid>", "false", "true" };
            auto property = m_property.to<dots::bool_t>();
            size_t itemIndex = property.isValid() + property.equal(true);

            if (ImGui::BeginCombo(m_inputLabel.data(), Items[itemIndex]))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, *m_color);
                if (ImGui::Selectable(Items[2], itemIndex == 2))
                {
                    property.constructOrAssign(true);
                    m_inputParsable = true;
                }
                if (ImGui::Selectable(Items[1], itemIndex == 1))
                {
                    property.constructOrAssign(false);
                    m_inputParsable = true;
                }
                ImGui::PopStyleColor();

                ImGui::EndCombo();
            }
        }
        else if (type == dots::type::Type::Enum)
        {
            const auto& enumDescriptor = m_property.descriptor().valueDescriptor().to<dots::type::EnumDescriptor<>>();
            const char* previewValue = m_property.isValid() ? enumDescriptor.enumeratorFromValue(*m_property).name().data() : "<invalid>";

            if (ImGui::BeginCombo(m_inputLabel.data(), previewValue))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, *m_color);
                for (const dots::type::EnumeratorDescriptor<>& enumeratorDescriptor : enumDescriptor.enumeratorsTypeless())
                {
                    const auto& value = enumeratorDescriptor.valueTypeless();

                    if (ImGui::Selectable(enumeratorDescriptor.name().data(), m_property == value))
                    {
                        m_property.constructOrAssign(value);
                        m_inputParsable = true;
                    }
                }
                ImGui::PopStyleColor();

                ImGui::EndCombo();
            }
        }
        else
        {
            if (ImGui::InputText(m_inputLabel.data(), m_buffer.data(), m_buffer.size()))
            {
                try
                {
                    std::string bufferNullTerminated = m_buffer.data();
                    dots::from_string(bufferNullTerminated, m_property);
                    m_inputParsable = true;
                }
                catch (...)
                {
                    m_inputParsable = false;
                }
            }
        }

        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::SameLine();
        if (ImGui::Button(m_invalidateLabel.data()))
        {
            constexpr char Invalid[] = "<invalid>";
            std::copy(Invalid, Invalid + sizeof Invalid, m_buffer.begin());
            m_inputParsable = true;
            m_property.destroy();
        }

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
