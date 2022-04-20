#include <components/HostPanel.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>

HostPanel::HostPanel(std::string appName, int argc, char** argv) :
    m_state(State::Pending),
    m_autoReconnect(false),
    m_appName{ std::move(appName) },
    m_argc(argc),
    m_argv(argv)
{
    /* do nothing */
}

void HostPanel::render()
{
    // update state
    update();

    // render panel
    {
        // render host label
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Host  ");
        }

        // render connection state
        {
            constexpr std::pair<const char*, ImVec4> StateStrs[] =
            {
                { "[pending]   ", ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
                { "[connecting]", ImVec4{ 1.0f, 0.75f, 0.0f, 1.0f } },
                { "[connected] ", ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
                { "[error]     ", ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f } }
            };

            auto [stateStr, stateColor] = StateStrs[static_cast<uint8_t>(m_state)];
            ImGui::SameLine();
            ImGui::TextColored(stateColor, stateStr);
        }

        // render reconnect button
        {
            constexpr char ConnectLabel[] = "Reconnect";
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

        // render auto reconnect option
        {
            ImGui::SameLine();
            ImGui::Checkbox("Auto", &m_autoReconnect);

            if (m_autoReconnect && m_state == State::Error)
            {
                m_state = State::Pending;
            }
        }

        // render pool view
        if (m_state == State::Connected)
        {
            ImGui::Separator();
            m_poolView->render();
        }
    }
}

void HostPanel::update()
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
