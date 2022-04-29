#include <widgets/PropertyEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

PropertyEdit::PropertyEdit(PropertyModel& model) :
    m_model{ model },
    m_inputLabel{ fmt::format("##PropertyEdit_{}_Input", static_cast<void*>(this)) },
    m_invalidateLabel{ fmt::format("X##PropertyEdit_{}_Invalidate", static_cast<void*>(this)) },
    m_randomizeLabel{ fmt::format("R##PropertyEdit_{}_Randomize", static_cast<void*>(this)) },
    m_timepointNowLabel{ fmt::format("N##PropertyEdit_{}_TimePointNow", static_cast<void*>(this)) }
{
    /* do nothing */
}

const PropertyModel& PropertyEdit::model() const
{
    return m_model;
}

PropertyModel& PropertyEdit::model()
{
    return m_model;
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
        ImGuiExt::TextColored(model().descriptorModel().declarationText());
    }

    // render input
    ImGui::TableNextColumn();
    PropertyModel& model = m_model.get();
    dots::type::ProxyProperty<>& property = model.property();

    if (dots::type::Type type = property.descriptor().valueDescriptor().type(); type != dots::type::Type::Struct)
    {
        bool valueChanged = model.valueChanged();
        const auto& [value, color] = model.valueText();

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.25f);

        if (type == dots::type::Type::boolean)
        {
            constexpr const char* Items[] = { "<invalid>", "false", "true" };
            auto boolProperty = property.to<dots::bool_t>();
            size_t itemIndex = boolProperty.isValid() + boolProperty.equal(true);

            if (ImGui::BeginCombo(m_inputLabel.data(), Items[itemIndex]))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                if (ImGui::Selectable(Items[2], itemIndex == 2))
                {
                    boolProperty.constructOrAssign(true);
                    model.fetch();
                    m_inputParseable = true;
                }
                if (ImGui::Selectable(Items[1], itemIndex == 1))
                {
                    boolProperty.constructOrAssign(false);
                    model.fetch();
                    m_inputParseable = true;
                }
                ImGui::PopStyleColor();

                ImGui::EndCombo();
            }
        }
        else if (type == dots::type::Type::Enum)
        {
            const auto& enumDescriptor = property.descriptor().valueDescriptor().to<dots::type::EnumDescriptor<>>();
            const char* previewValue = property.isValid() ? enumDescriptor.enumeratorFromValue(*property).name().data() : "<invalid>";

            if (ImGui::BeginCombo(m_inputLabel.data(), previewValue))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                for (const dots::type::EnumeratorDescriptor<>& enumeratorDescriptor : enumDescriptor.enumeratorsTypeless())
                {
                    const auto& enumerator = enumeratorDescriptor.valueTypeless();

                    if (ImGui::Selectable(enumeratorDescriptor.name().data(), property == enumerator))
                    {
                        property.constructOrAssign(enumerator);
                        model.fetch();
                        m_inputParseable = true;
                    }
                }
                ImGui::PopStyleColor();

                ImGui::EndCombo();
            }
        }
        else
        {
            if (valueChanged)
            {
                m_inputBuffer.assign(std::max(value.size(), size_t{ 256 }), '\0');
                std::copy(value.begin(), value.end(), m_inputBuffer.begin());
            }

            if (ImGui::InputText(m_inputLabel.data(), m_inputBuffer.data(), m_inputBuffer.size(), ImGuiInputTextFlags_AutoSelectAll))
            {
                m_inputParseable = model.fromString(m_inputBuffer.data());
            }
        }

        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::SameLine();
        if (ImGui::Button(m_invalidateLabel.data()))
        {
            model.invalidate();
            m_inputParseable = true;
        }
        ImGuiExt::TooltipLastHoveredItem("Invalidate property");

        ImGui::SameLine();
        if (ImGui::Button(m_randomizeLabel.data()))
        {
            model.randomize();
            m_inputParseable = true;
        }
        ImGuiExt::TooltipLastHoveredItem("Randomize property");

        ImVec2 randomizeButtonSize = ImGui::GetItemRectSize();

        if (type == dots::type::Type::timepoint || type == dots::type::Type::steady_timepoint)
        {
            ImGui::SameLine();
            if (ImGui::Button(m_timepointNowLabel.data()))
            {
                if (type == dots::type::Type::timepoint)
                {
                    auto timepointProperty = property.to<dots::timepoint_t>();
                    timepointProperty.constructOrAssign(dots::timepoint_t::Now());
                }
                else/* if (type == dots::type::Type::steady_timepoint)*/
                {
                    auto steadyTimepointProperty = property.to<dots::steady_timepoint_t>();
                    steadyTimepointProperty.constructOrAssign(dots::steady_timepoint_t::Now());
                }

                model.fetch();
                m_inputParseable = true;
            }
            ImGuiExt::TooltipLastHoveredItem("Set to 'now' (i.e. the current time)");
        }
        else
        {
            ImGui::SameLine();
            ImGui::Dummy(randomizeButtonSize);
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
