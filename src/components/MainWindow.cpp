#include <components/MainWindow.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>

MainWindow::MainWindow(std::string appName, int argc, char** argv) :
    m_hostPanel{ std::move(appName), argc, argv }
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
            m_hostPanel.render();
        }

        ImGui::End();
    }
}
