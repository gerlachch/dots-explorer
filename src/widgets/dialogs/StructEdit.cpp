#include <widgets/dialogs/StructEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

StructEdit::StructEdit(const StructDescriptorModel& structDescriptorModel, dots::type::AnyStruct instance) :
    m_popupId{ fmt::format("StructEdit-{}_Popup", ++M_id) },
    m_instance{ std::move(instance) },
    m_structRefModel{ structDescriptorModel, m_instance }
{
    for (PropertyModel& propertyModel : m_structRefModel.propertyModels())
    {
        m_propertyEdits.emplace_back(propertyModel);
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
                            ImGuiExt::TooltipLastHoveredItem("The publish will probably create a new instance identified by this key property.");
                        }
                        else
                        {
                            if (isValid)
                            {
                                ImGuiExt::TooltipLastHoveredItem("The publish will probably create this property.");
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
                                ImGuiExt::TooltipLastHoveredItem("The publish will probably update the existing instance identified by this key property.");
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
                                ImGuiExt::TooltipLastHoveredItem("The publish will probably update this property.");
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
            bool noErrors = std::none_of(m_propertyEdits.begin(), m_propertyEdits.end(), [](const PropertyEdit& propertyEdit)
            {
                return propertyEdit.included() == false;
            });

            if (noErrors && m_instance->_hasProperties(m_instance->_keyProperties()))
            {
                if (ImGui::Button("Publish"))
                {
                    dots::property_set_t includedProperties;

                    for (const PropertyEdit& propertyEdit : m_propertyEdits)
                    {
                        if (propertyEdit.included() == true)
                        {
                            const dots::type::PropertyPath& propertyPath = propertyEdit.model().descriptorModel().propertyPath();
                            includedProperties += propertyPath.elements().front().get().set();
                        }
                    }

                    dots::publish(m_instance, includedProperties);
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
