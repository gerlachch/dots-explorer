#include <components/HostPanel.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>
#include <dots/io/Io.h>
#include <common/Colors.h>
#include <DotsDescriptorRequest.dots.h>

HostPanel::HostPanel(std::string appName) :
    m_state(State::Disconnected),
    m_autoConnect(false),
    m_endpointBuffer(256, '\0'),
    m_appName{ std::move(appName) }
{
    std::string_view defaultHost = "tcp://127.0.0.1:11234";
    std::copy(defaultHost.begin(), defaultHost.end(), m_endpointBuffer.begin());
}

HostPanel::~HostPanel()
{
    disconnect();
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

        // render host input
        {
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::InputTextWithHint("##Host", "<host-endpoint>",  m_endpointBuffer.data(), m_endpointBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                m_state = State::Pending;
            }
            ImGui::PopItemWidth();
        }

        // render auto connect option
        {
            ImGui::SameLine();
            ImGui::Checkbox("Auto", &m_autoConnect);
        }

        // render connect button
        {
            ImGui::SameLine();

            if (ImGui::Button("Connect") || (m_autoConnect && (m_state == State::Disconnected || m_state == State::Error)))
            {
                disconnect();
                m_state = State::Pending;
            }
        }

        // render connection state
        {
            constexpr std::pair<const char*, ImVec4> StateStrs[] =
            {
                { "[disconnected]", ColorThemeActive.Disabled },
                { "[pending]     ", ColorThemeActive.Pending },
                { "[connecting]  ", ColorThemeActive.Pending },
                { "[connected]   ", ColorThemeActive.Success },
                { "[error]       ", ColorThemeActive.Error }
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
}

void HostPanel::disconnect()
{
    boost::asio::io_context& ioContext = dots::io::global_io_context();

    try
    {
        ioContext.stop();
        dots::global_transceiver().reset();
        ioContext.restart();
        ioContext.poll();
    }
    catch (...)
    {
    }

    ioContext.restart();
    m_state = State::Disconnected;
}

void HostPanel::update()
{
    switch (m_state)
    {
        case State::Disconnected:
            break;
        case State::Pending:
            m_connectTask = std::async(std::launch::async, [this]
            {
                using transition_handler_t = dots::GuestTransceiver::transition_handler_t;
                dots::GuestTransceiver& transceiver = dots::global_transceiver().emplace(
                    m_appName,
                    dots::io::global_io_context(),
                    dots::type::Registry::StaticTypePolicy::All,
                    transition_handler_t{ &HostPanel::handleTransceiverTransition, this }
                );
                transceiver.open(dots::io::Endpoint{ m_endpointBuffer.data() });

                for (;;)
                {
                    if (m_connectionError != nullptr)
                    {
                        std::rethrow_exception(m_connectionError);
                    }

                    if (transceiver.connected())
                    {
                        break;
                    }
                    else
                    {
                        dots::global_transceiver()->ioContext().run_one();
                    }
                }
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
            dots::global_transceiver()->ioContext().poll();
            break;
        case State::Error:
            break;
    }
}

void HostPanel::handleTransceiverTransition(const dots::Connection& connection, std::exception_ptr ePtr)
{
    m_connectionError = ePtr;

    if (connection.closed())
    {
        m_state = State::Error;
    }
}
