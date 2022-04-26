#include <widgets/StructEdit.h>
#include <algorithm>
#include <imgui.h>
#include <fmt/format.h>

StructEdit::StructEdit(const StructDescriptorModel& structDescriptorModel, dots::type::AnyStruct instance) :
    m_popupId{ fmt::format("StructEdit-{}_Popup", ++M_id) },
    m_instance{ std::move(instance) },
    m_structModel{ structDescriptorModel, m_instance }
{
    for (PropertyModel& propertyModel : m_structModel.propertyModels())
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
            ImGuiExt::TextColored(m_structModel.descriptorModel().declarationText());
            ImGui::Separator();
        }

        // properties
        if (ImGui::BeginTable("StructEditTable", 2))
        {
            for (PropertyEdit& propertyEdit : m_propertyEdits)
            {
                propertyEdit.render();
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
                            includedProperties += propertyEdit.model().property().descriptor().set();
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
