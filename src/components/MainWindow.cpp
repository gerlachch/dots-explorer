#include <components/MainWindow.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>

MainWindow::MainWindow(std::string appName, int argc, char** argv) :
    m_state(State::Pending),
    m_appName{ std::move(appName) },
    m_argc(argc),
    m_argv(argv)
{
    /* do nothing */
}

void MainWindow::render()
{
    // update state
    update();

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

            // render connect button
            {
                constexpr char ConnectLabel[] = "Connect";
                ImGui::SameLine();

                if (m_state == State::Error)
                {
                    if (ImGui::Button(ConnectLabel))
                    {
                        m_state = State::Pending;
                    }
                }
                else
                {
                    ImGui::BeginDisabled();
                    ImGui::Button(ConnectLabel);
                    ImGui::EndDisabled();
                }
            }

            // render connection state
            {
                constexpr std::pair<const char*, ImVec4> StateStrs[] =
                {
                    { "[pending]   ", ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
                    { "[connecting]", ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
                    { "[connected] ", ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
                    { "[error]     ", ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f } }
                };

                auto [stateStr, stateColor] = StateStrs[static_cast<uint8_t>(m_state)];
                ImGui::SameLine();
                ImGui::TextColored(stateColor, stateStr);
            }

            // render pool view
            if (m_state == State::Connected)
            {
                ImGui::Separator();
                m_poolView->render();
            }
        }

        ImGui::End();
    }
}

void MainWindow::update()
{
    switch (m_state)
    {
        case State::Pending:
            m_connectTask = std::async(std::launch::async, [this]
            {
                m_application.emplace(m_appName, m_argc, m_argv);
            });
            m_state = State::Connecting;
            break;
        case State::Connecting:
            try
            {
                if (auto status = m_connectTask->wait_for(std::chrono::milliseconds{ 5 }); status == std::future_status::ready)
                {
                    m_connectTask->get();
                    m_poolView.emplace();
                    dots::publish(DotsDescriptorRequest{});
                    m_state = State::Connected;
                }
            }
            catch (...)
            {
                m_state = State::Error;
            }
            break;
        case State::Connected:
            m_application->transceiver().ioContext().poll();
            break;
        case State::Error:
            break;
    }
}
