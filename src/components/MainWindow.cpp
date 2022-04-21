#include <components/MainWindow.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>

MainWindow::MainWindow(std::string appName) :
    m_hostPanel{ std::move(appName) }
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
            // render host panel
            m_hostPanel.render();
        }

        ImGui::End();
    }
}
