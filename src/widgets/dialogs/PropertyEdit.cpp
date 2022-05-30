#include <widgets/dialogs/PropertyEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

PropertyEdit::PropertyEdit(PropertyModel& model, std::optional<bool> included/* = std::nullopt*/) :
    m_model{ model },
    m_inputLabel{ fmt::format("##PropertyEdit_{}_Input", static_cast<void*>(this)) },
    m_excludeLabel{ fmt::format("E##PropertyEdit_{}_Exclude", static_cast<void*>(this)) },
    m_invalidateLabel{ fmt::format("X##PropertyEdit_{}_Invalidate", static_cast<void*>(this)) },
    m_randomizeLabel{ fmt::format("R##PropertyEdit_{}_Randomize", static_cast<void*>(this)) },
    m_timepointNowLabel{ fmt::format("N##PropertyEdit_{}_TimePointNow", static_cast<void*>(this)) },
    m_included{ included }
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

std::optional<bool> PropertyEdit::included() const
{
    return m_included;
}

void PropertyEdit::render()
{
    ImGui::TableNextRow();

    // render description
    {
        ImGui::TableNextColumn();
        ImGuiExt::TextColored(model().descriptorModel().declarationText());
    }

    // render edit area
    if (dots::type::Type type = model().property().descriptor().valueDescriptor().type(); type != dots::type::Type::Struct)
    {
        ImGui::TableNextColumn();

        PropertyModel& model = m_model.get();
        dots::type::ProxyProperty<>& property = model.property();

        // render input field
        {
            bool valueChanged = model.valueChanged();
            auto [value, color] = model.valueText();

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x * 0.25f);

            if (type == dots::type::Type::boolean)
            {
                constexpr const char* Items[] = { "<invalid>", "false", "true" };
                auto boolProperty = property.to<dots::bool_t>();
                size_t itemIndex = boolProperty.isValid() + boolProperty.equal(true);

                if (ImGui::BeginCombo(m_inputLabel.data(), Items[itemIndex]))
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.Keyword);
                    if (ImGui::Selectable(Items[2], itemIndex == 2))
                    {
                        boolProperty.constructOrAssign(true);
                        model.fetch();
                        m_included = true;
                    }
                    if (ImGui::Selectable(Items[1], itemIndex == 1))
                    {
                        boolProperty.constructOrAssign(false);
                        model.fetch();
                        m_included = true;
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
                    ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.EnumType);
                    for (const dots::type::EnumeratorDescriptor<>& enumeratorDescriptor : enumDescriptor.enumeratorsTypeless())
                    {
                        const auto& enumerator = enumeratorDescriptor.valueTypeless();

                        if (ImGui::Selectable(enumeratorDescriptor.name().data(), property == enumerator))
                        {
                            property.constructOrAssign(enumerator);
                            model.fetch();
                            m_included = true;
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

                    if (type == dots::type::Type::string)
                    {
                        std::copy(value.begin() + 1, value.end() - 1, m_inputBuffer.begin());
                    }
                    else
                    {
                        std::copy(value.begin(), value.end(), m_inputBuffer.begin());
                    }
                }

                if (ImGui::InputText(m_inputLabel.data(), m_inputBuffer.data(), m_inputBuffer.size(), ImGuiInputTextFlags_AutoSelectAll))
                {
                    m_included = model.fromString(m_inputBuffer.data());
                }
            }

            ImGui::PopItemWidth();
            ImGui::PopStyleColor();
        }

        // render 'Exclude' button
        {
            ImGui::BeginDisabled(property.descriptor().isKey() || m_included == std::nullopt);
            ImGui::SameLine();

            if (ImGui::Button(m_excludeLabel.data()))
            {
                m_included = std::nullopt;
            }

            ImGuiExt::TooltipLastHoveredItem("Exclude property from publish");
            ImGui::EndDisabled();
        }

        // render 'Invalidate' button
        {
            ImGui::BeginDisabled(property.descriptor().isKey() || !property.isValid());
            ImGui::SameLine();

            if (ImGui::Button(m_invalidateLabel.data()))
            {
                model.invalidate();
                m_included = true;
            }

            ImGuiExt::TooltipLastHoveredItem("Invalidate property");
            ImGui::EndDisabled();
        }

        // render 'Randomize' button
        {
            ImGui::SameLine();

            if (ImGui::Button(m_randomizeLabel.data()))
            {
                model.randomize();
                m_included = true;
            }

            ImGuiExt::TooltipLastHoveredItem("Randomize property");
        }

        // render time point 'Now' button
        {
            ImVec2 randomizeButtonSize = ImGui::GetItemRectSize();
            ImGui::SameLine();

            if (type == dots::type::Type::timepoint || type == dots::type::Type::steady_timepoint)
            {
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
                    m_included = true;
                }
                ImGuiExt::TooltipLastHoveredItem("Set to 'now' (i.e. the current time)");
            }
            else
            {
                ImGui::Dummy(randomizeButtonSize);
            }
        }
    }
}
