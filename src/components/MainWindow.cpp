#include <components/MainWindow.h>
#include <imgui.h>

void MainWindow::render()
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
        m_containerPool.render();
    }

    ImGui::End();
}
