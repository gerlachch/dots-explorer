#include <widgets/PropertyEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

PropertyEdit::PropertyEdit(dots::type::Struct& instance, const dots::type::PropertyPath& propertyPath) :
    m_property{ instance, propertyPath },
    m_propertyDescription{ propertyPath },
    m_inputLabel{ fmt::format("##PropertyEdit_{}_Input", m_property.descriptor().name()) },
    m_invalidateLabel{ fmt::format("X##PropertyEdit_{}_Invalidate", m_property.descriptor().name()) },
    m_randomizeLabel{ fmt::format("R##PropertyEdit_{}_Randomize", m_property.descriptor().name()) }
{
    // init input buffer
    {
        std::string value = dots::to_string(m_property);
        m_inputBuffer.assign(std::max(value.size(), size_t{ 256 }), '\0');
        std::copy(value.begin(), value.end(), m_inputBuffer.begin());
    }

    // init input colors
    switch (m_property.descriptor().valueDescriptor().type())
    {
        case dots::type::Type::boolean:
            m_inputColor = ColorThemeActive.Keyword;
            break;
        case dots::type::Type::string:
            m_inputColor = ColorThemeActive.StringType;
            break;
        case dots::type::Type::Enum:
            m_inputColor = ColorThemeActive.EnumType;
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
        case dots::type::Type::property_set:
        case dots::type::Type::timepoint:
        case dots::type::Type::steady_timepoint:
        case dots::type::Type::duration:
        case dots::type::Type::uuid:
        case dots::type::Type::Vector:
        case dots::type::Type::Struct:
        default:
            m_inputColor = ColorThemeActive.IntegralType;
            break;
    }
}

const dots::type::ProxyProperty<>& PropertyEdit::property() const
{
    return m_property;
}

std::optional<bool> PropertyEdit::inputParseable() const
{
    return m_inputParseable;
}

void PropertyEdit::render()
{
    ImGui::TableNextRow();

    // render description
    {
        ImGui::TableNextColumn();
        m_propertyDescription.render();
    }

    // render input
    ImGui::TableNextColumn();
    if (dots::type::Type type = m_property.descriptor().valueDescriptor().type(); type != dots::type::Type::Struct)
    {
        if (m_property.isValid())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, *m_inputColor);
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.Disabled);
        }
        ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.25f);

        if (type == dots::type::Type::boolean)
        {
            constexpr const char* Items[] = { "<invalid>", "false", "true" };
            auto property = m_property.to<dots::bool_t>();
            size_t itemIndex = property.isValid() + property.equal(true);

            if (ImGui::BeginCombo(m_inputLabel.data(), Items[itemIndex]))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, *m_inputColor);
                if (ImGui::Selectable(Items[2], itemIndex == 2))
                {
                    property.constructOrAssign(true);
                    m_inputParseable = true;
                }
                if (ImGui::Selectable(Items[1], itemIndex == 1))
                {
                    property.constructOrAssign(false);
                    m_inputParseable = true;
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
                ImGui::PushStyleColor(ImGuiCol_Text, *m_inputColor);
                for (const dots::type::EnumeratorDescriptor<>& enumeratorDescriptor : enumDescriptor.enumeratorsTypeless())
                {
                    const auto& value = enumeratorDescriptor.valueTypeless();

                    if (ImGui::Selectable(enumeratorDescriptor.name().data(), m_property == value))
                    {
                        m_property.constructOrAssign(value);
                        m_inputParseable = true;
                    }
                }
                ImGui::PopStyleColor();

                ImGui::EndCombo();
            }
        }
        else
        {
            if (ImGui::InputText(m_inputLabel.data(), m_inputBuffer.data(), m_inputBuffer.size()))
            {
                try
                {
                    std::string bufferNullTerminated = m_inputBuffer.data();
                    dots::from_string(bufferNullTerminated, m_property);
                    m_inputParseable = true;
                }
                catch (...)
                {
                    m_inputParseable = false;
                }
            }
        }

        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::SameLine();
        if (ImGui::Button(m_invalidateLabel.data()))
        {
            constexpr char Invalid[] = "<invalid>";
            std::copy(Invalid, Invalid + sizeof Invalid, m_inputBuffer.begin());
            m_inputParseable = true;
            m_property.destroy();
        }

        ImGui::SameLine();
        if (ImGui::Button(m_randomizeLabel.data()))
        {
            if (m_randomizer == std::nullopt)
            {
                m_randomizer.emplace(std::random_device{}());
            }

            m_randomizer->randomize(m_property);
            std::string value = dots::to_string(m_property);
            m_inputBuffer.assign(std::max(value.size(), m_inputBuffer.size()), '\0');
            std::copy(value.begin(), value.end(), m_inputBuffer.begin());
            
            m_inputParseable = true;
        }

        ImGui::SameLine();
        if (m_inputParseable != std::nullopt)
        {
            if (*m_inputParseable)
            {
                ImGui::TextColored(ColorThemeActive.Success, "Ok   ");
            }
            else
            {
                ImGui::TextColored(ColorThemeActive.Error, "Error");
            }
        }
        else
        {
            ImGui::TextUnformatted("     ");
        }
    }
}
