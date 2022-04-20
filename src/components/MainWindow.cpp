#include <components/MainWindow.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>

MainWindow::MainWindow(std::string appName) :
    m_endpointBuffer(256, '\0'),
    m_appName{ std::move(appName) }
{
    /* do nothing */
}

void MainWindow::render()
{
    // render window
    {
        constexpr ImGuiWindowFlags WindowFlags =
           ImGuiWindowFlags_NoTitleBar |
           ImGuiWindowFlags_NoResize   |
           ImGuiWindowFlags_NoMove
        ;

        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowPos(ImVec2{ 0.0f, 0.0f });

        if (ImGui::Begin("MainWindow", nullptr, WindowFlags))
        {
            // render host label
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Host  ");
            }
            
            // render host input
            {
                ImGui::SameLine();
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
                if (ImGui::InputTextWithHint("##Host", "<host-endpoint>",  m_endpointBuffer.data(), m_endpointBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    m_hostPanel.emplace(m_appName, m_endpointBuffer.data());
                }
                ImGui::PopItemWidth();
            }

            if (m_hostPanel != std::nullopt)
            {
                ImGui::SameLine();
                m_hostPanel->render();
            }
        }

        ImGui::End();
    }
}
