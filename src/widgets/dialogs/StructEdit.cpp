#include <widgets/dialogs/StructEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

StructEdit::StructEdit(const StructDescriptorModel& structDescriptorModel, dots::type::AnyStruct instance) :
    m_popupId{ fmt::format("StructEdit-{}_Popup", ++M_id) },
    m_remove(false),
    m_instance{ std::move(instance) },
    m_structRefModel{ structDescriptorModel, m_instance }
{
    const dots::type::StructDescriptor<>& structDescriptor = structDescriptorModel.descriptor();

    for (PropertyModel& propertyModel : m_structRefModel.propertyModels())
    {
        const dots::type::PropertyDescriptor& propertyDescriptor = propertyModel.descriptorModel().propertyPath().destination();

        m_propertyEdits.emplace_back(
            propertyModel,
            propertyModel.property().isValid() && (propertyDescriptor.isKey() || !structDescriptor.cached())
                ? std::optional{ true }
                : std::nullopt
        );
    }

    ImGui::OpenPopup(m_popupId.data());
}

bool StructEdit::render()
{
    ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, 0, ImVec2{ 0.5f, 0.5f });

    if (ImGui::BeginPopup(m_popupId.data(), ImGuiWindowFlags_NoMove))
    {
        // description
        {
            ImGuiExt::TextColored(m_structRefModel.descriptorModel().declarationText());
            ImGui::Separator();
        }

        // properties
        const dots::type::StructDescriptor<>& structDescriptor = m_instance->_descriptor();
        auto* existingInstance = m_instance->_hasProperties(structDescriptor.keyProperties())
            ? dots::container(structDescriptor).find(m_instance)
            : nullptr
        ;

        if (ImGui::BeginTable("StructEditTable", 2))
        {
            for (PropertyEdit& propertyEdit : m_propertyEdits)
            {
                propertyEdit.render();

                // render input indicator
                const dots::type::ProxyProperty<>& property = propertyEdit.model().property();

                bool isKey = property.descriptor().isKey();
                bool isValid = property.isValid();
                bool isExcluded = propertyEdit.included() == std::nullopt;
                bool hasError = !isExcluded && !*propertyEdit.included();

                if (isKey && !isValid)
                {
                    ImGui::SameLine();
                    ImGui::TextColored(ColorThemeActive.Error, "! ");
                    ImGuiExt::TooltipLastHoveredItem("Key properties must always be valid.");
                }
                else if (hasError)
                {
                    ImGui::SameLine();
                    ImGui::TextColored(ColorThemeActive.Error, "! ");
                    ImGuiExt::TooltipLastHoveredItem("The property could not be created from the specified input.");
                }
                else if (isExcluded)
                {
                    ImGui::SameLine();
                    ImGui::TextUnformatted("  ");
                }
                else
                {
                    auto render_indicator = [](const ImVec4& color)
                    {
                        ImGui::SameLine(0, 0);
                        ImGui::BeginGroup();
                        ImGui::PushStyleColor(ImGuiCol_Text, color);
                        ImGui::Bullet();
                        ImGui::PopStyleColor();
                        ImGui::TextUnformatted("");
                        ImGui::EndGroup();
                    };

                    const dots::type::PropertyPath& propertyPath = propertyEdit.model().descriptorModel().propertyPath();

                    if (existingInstance == nullptr || !dots::type::ProxyProperty{ const_cast<dots::type::Struct&>(*existingInstance), propertyPath }.isValid())
                    {
                        render_indicator(ColorThemeActive.Create);

                        if (isKey)
                        {
                            ImGuiExt::TooltipLastHoveredItem("The publish will create a new instance identified by this key property.");
                        }
                        else
                        {
                            if (isValid)
                            {
                                ImGuiExt::TooltipLastHoveredItem("The publish will create this property.");
                            }
                            else
                            {
                                ImGuiExt::TooltipLastHoveredItem("The publish will include invalidation of this property, but it will probably have no effect.");
                            }
                        }
                    }
                    else
                    {
                        const dots::type::ProxyProperty existingProperty{ const_cast<dots::type::Struct&>(*existingInstance), propertyPath };

                        if (property.equal(existingProperty))
                        {
                            render_indicator(ColorThemeActive.Update);

                            if (isKey)
                            {
                                ImGuiExt::TooltipLastHoveredItem("The publish will update the existing instance identified by this key property.");
                            }
                            else
                            {
                                 ImGuiExt::TooltipLastHoveredItem("The publish will include an update of this property, but it will probably have no effect.");
                            }
                        }
                        else
                        {
                            if (isValid)
                            {
                                render_indicator(ColorThemeActive.Update);
                                ImGuiExt::TooltipLastHoveredItem("The publish will update this property.");
                            }
                            else
                            {
                                render_indicator(ColorThemeActive.Remove);
                                ImGuiExt::TooltipLastHoveredItem("The publish will invalidate this property.");
                            }
                        }
                    }
                }
            }
            
            ImGui::EndTable();
        }

        // buttons
        {
            bool errors = std::any_of(m_propertyEdits.begin(), m_propertyEdits.end(), [](const PropertyEdit& propertyEdit)
            {
                return propertyEdit.included() == false;
            });

            dots::property_set_t includedProperties;

            for (const PropertyEdit& propertyEdit : m_propertyEdits)
            {
                if (propertyEdit.included() == true)
                {
                    const dots::type::PropertyPath& propertyPath = propertyEdit.model().descriptorModel().propertyPath();
                    includedProperties += propertyPath.elements().front().get().set();
                }
            }

            ImGui::BeginDisabled(errors || !m_instance->_hasProperties(m_instance->_keyProperties()));
            {
                if (ImGui::Button("Publish"))
                {
                    dots::publish(m_instance, includedProperties, m_remove);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            if (m_instance->_descriptor().cached())
            {
                ImGui::SameLine();
                ImGui::Checkbox("Remove", &m_remove);
                ImGuiExt::TooltipLastHoveredItem(
                    "Setting this will publish the instance with the remove flag.\n\n"
                    "Note that contrary to other ways of removing instances, the resulting publish will include non-key properties."
                );
            }

            ImGui::SameLine();
            if (!m_instance->_hasProperties(m_instance->_keyProperties()))
            {
                ImGui::TextColored(ColorThemeActive.Error, "[missing keys]");
                ImGuiExt::TooltipLastHoveredItem("At lest one key property does not have a valid value.");
            }
            else if (errors)
            {
                ImGui::TextColored(ColorThemeActive.Error, "[input error]");
                ImGuiExt::TooltipLastHoveredItem("At least one property could not be created from the specified input.");
            }
            else
            {
                size_t includedCount = includedProperties.count();
                std::string includedCountStr = includedCount == 1
                    ? std::string{ "1 property" }
                    : fmt::format("{} properties", includedCount)
                ;

                if (m_remove)
                {
                    ImGui::TextColored(ColorThemeActive.Remove, "[%zu selected]", includedCount);

                    if (existingInstance == nullptr)
                    {
                        ImGuiExt::TooltipLastHoveredItem(fmt::format("The publish will contain {} and attempt to remove an existing instance, but it will probably have no effect.", includedCountStr));
                    }
                    else
                    {
                        ImGuiExt::TooltipLastHoveredItem(fmt::format("The publish will contain {} and attempt to remove the existing instance.", includedCountStr));
                    }
                }
                else if (existingInstance == nullptr)
                {
                    ImGui::TextColored(ColorThemeActive.Create, "[%zu selected]", includedCount);
                    ImGuiExt::TooltipLastHoveredItem(fmt::format("The publish will contain {} and create a new instance.", includedCountStr));
                }
                else
                {
                    ImGui::TextColored(ColorThemeActive.Update, "[%zu selected]", includedCount);
                    ImGuiExt::TooltipLastHoveredItem(fmt::format("The publish will contain {} and update the existing instance.", includedCountStr));
                }
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
