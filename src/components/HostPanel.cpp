#include <components/HostPanel.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>
#include <dots/io/Io.h>
#include <common/Colors.h>
#include <common/Settings.h>
#include <DotsDescriptorRequest.dots.h>

HostPanel::HostPanel(std::string appName) :
    m_state(State::Disconnected),
    m_selectedHost(nullptr),
    m_deltaSinceError(0.0f),
    m_hostSettings{ Settings::Register<HostSettings>() },
    m_appName{ std::move(appName) }
{
    /* do nothing */
}

HostPanel::~HostPanel()
{
    disconnect();
}

void HostPanel::render()
{
    // update state
    update();

    bool openHostSettingsEdit = false;
    Host* editHost = nullptr;

    // render panel
    {
        // render host label
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Host  ");
        }

        dots::vector_t<Host>& hosts = m_hostSettings.hosts.constructOrValue();

        // ensure hosts are valid
        {
            if (hosts.empty() || !std::all_of(hosts.begin(), hosts.end(), [](const Host& host){ return host._hasProperties(host._properties()); }))
            {
                hosts.clear();
                hosts.emplace_back(Host::endpoint_i{ "tcp://127.0.0.1:11234" }, Host::description_i{ "localhost (default)" });
            }

            if (!m_hostSettings.selectedHost.isValid() || *m_hostSettings.selectedHost >= hosts.size())
            {
                m_hostSettings.selectedHost = 0;
            }

            m_hostSettings.autoConnect.constructOrValue();
        }

        ImGui::BeginDisabled(m_state == State::Connecting);
        m_selectedHost = &hosts[m_hostSettings.selectedHost];

        // render hosts list
        {
            auto create_host_label = [this](const Host& host)
            {
                m_hostLabel.clear();
                m_hostLabel += host.description;
                m_hostLabel += " [";
                m_hostLabel += host.endpoint;
                m_hostLabel += "]";
                return m_hostLabel.data();
            };

            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::BeginCombo("##Hosts", create_host_label(*m_selectedHost)))
            {
                if (ImGui::Selectable("<New"))
                {
                    openHostSettingsEdit = true;
                }

                if (ImGui::Selectable("<Edit>"))
                {
                    openHostSettingsEdit = true;
                    editHost = m_selectedHost;
                }

                if (hosts.size() > 1)
                {
                    if (ImGui::Selectable("<Remove>"))
                    {
                        hosts.erase(hosts.begin() + m_hostSettings.selectedHost);

                        if (*m_hostSettings.selectedHost >= hosts.size())
                        {
                            --*m_hostSettings.selectedHost;
                            m_selectedHost = &hosts[m_hostSettings.selectedHost];
                        }
                    }
                }

                ImGui::Separator();

                uint32_t i = 0;

                for (Host& host : hosts)
                {
                    if (ImGui::Selectable(create_host_label(host), i == m_hostSettings.selectedHost) && i != m_hostSettings.selectedHost)
                    {
                        m_hostSettings.selectedHost = i;
                        m_selectedHost = &host;
                        m_state = State::Pending;
                    }

                    ++i;
                }

                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
        }

        // render auto connect option
        {
            ImGui::SameLine();
            ImGui::Checkbox("Auto", &*m_hostSettings.autoConnect);

            if (*m_hostSettings.autoConnect && (m_state == State::Disconnected || m_state == State::Error && m_deltaSinceError > 5.0f))
            {
                m_state = State::Pending;
            }
        }

        // render connect button
        {
            ImGui::SameLine();

            if (ImGui::Button("Connect"))
            {
                m_state = State::Pending;
            }
        }
        ImGui::EndDisabled();

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

        ImGui::Separator();

        // render pool view
        if (m_state == State::Connected)
        {
            m_poolView->render();
        }
    }

    // render host settings edit
    {
        if (openHostSettingsEdit)
        {
            m_hostSettingsEdit.emplace(m_hostSettings, editHost);
        }

        if (m_hostSettingsEdit != std::nullopt && !m_hostSettingsEdit->render())
        {
            m_hostSettingsEdit = std::nullopt;
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
            disconnect();
            m_connectTask = std::async(std::launch::async, [this]
            {
                using transition_handler_t = dots::GuestTransceiver::transition_handler_t;
                dots::GuestTransceiver& transceiver = dots::global_transceiver().emplace(
                    m_appName,
                    dots::io::global_io_context(),
                    dots::type::Registry::StaticTypePolicy::All,
                    transition_handler_t{ &HostPanel::handleTransceiverTransition, this }
                );
                transceiver.open(dots::io::Endpoint{ m_selectedHost->endpoint->data() });

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
                m_deltaSinceError = 0.0f;
                m_state = State::Error;
            }
            break;
        case State::Connected:
            dots::global_transceiver()->ioContext().poll();
            break;
        case State::Error:
            m_deltaSinceError += ImGui::GetIO().DeltaTime;
            break;
    }
}

void HostPanel::handleTransceiverTransition(const dots::Connection& connection, std::exception_ptr ePtr)
{
    m_connectionError = ePtr;

    if (connection.closed())
    {
        m_deltaSinceError = 0.0f;
        m_state = State::Error;
    }
}
