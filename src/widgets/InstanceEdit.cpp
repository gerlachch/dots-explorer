#include <widgets/InstanceEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>

InstanceEdit::InstanceEdit(dots::type::AnyStruct instance) :
    m_popupId{ fmt::format("InstanceEdit-{}_Popup", ++M_id) },
    m_instance{ std::move(instance) }
{
    for (const dots::type::PropertyPath& propertyPath : m_instance->_descriptor().propertyPaths())
    {
        m_propertyEdits.emplace_back(m_instance, propertyPath);
    }

    ImGui::OpenPopup(m_popupId.data());
}

bool InstanceEdit::render(const StructDescription& structDescription, const std::vector<PropertyDescription>& propertyDescriptions)
{
    ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, 0, ImVec2{ 0.5f, 0.5f });

    if (ImGui::BeginPopup(m_popupId.data(), ImGuiWindowFlags_NoMove))
    {
        // description
        {
            structDescription.render();
            ImGui::Separator();
        }

        // properties
        if (ImGui::BeginTable("InstanceEditTable", 2))
        {
            auto it = propertyDescriptions.begin();

            for (PropertyEdit& propertyEdit : m_propertyEdits)
            {
                propertyEdit.render(*it++);
            }
            
            ImGui::EndTable();
        }

        // buttons
        {
            bool allInputParseable = std::none_of(m_propertyEdits.begin(), m_propertyEdits.end(), [](const PropertyEdit& propertyEdit)
            {
                return propertyEdit.inputParseable() == false;
            });

            if (allInputParseable)
            {
                if (ImGui::Button("Publish"))
                {
                    dots::property_set_t includedProperties;

                    for (const PropertyEdit& propertyEdit : m_propertyEdits)
                    {
                        if (propertyEdit.inputParseable() == true)
                        {
                            includedProperties += propertyEdit.property().descriptor().set();
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
