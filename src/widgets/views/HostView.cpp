#include <widgets/views/HostView.h>
#include <boost/asio.hpp>
#include <imgui.h>
#include <fmt/format.h>
#include <dots/io/Io.h>
#include <dots/io/channels/LocalListener.h>
#include <dots_ext/FileInChannel.h>
#include <common/Colors.h>
#include <common/Settings.h>
#include <common/System.h>
#include <DotsDescriptorRequest.dots.h>

HostView::HostView(std::string appName) :
    m_state(State::Disconnected),
    m_selectedHost(nullptr),
    m_deltaSinceError(0.0f),
    m_helpHintWidth(100.0f),
    m_hostSettings{ Settings::Register<HostSettings>() },
    m_viewSettings{ Settings::Register<ViewSettings>() },
    m_releaseInfoTask{ Version::GetReleaseInfo() },
    m_appName{ std::move(appName) }
{
    /* do nothing */
}

HostView::~HostView()
{
    disconnect();
}

void HostView::render()
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

        dots::vector_t<Host>& hosts = m_hostSettings.hosts;

        // ensure hosts are valid
        {
            if (hosts.empty())
            {
                hosts.emplace_back(Host::endpoint_i{ "tcp://127.0.0.1:11234" }, Host::description_i{ "localhost (default)" });
            }

            if (*m_hostSettings.selectedHost >= hosts.size())
            {
                m_hostSettings.selectedHost = 0;
            }
        }

        // check dropped files
        if (!System::DroppedFiles.empty())
        {
            size_t hostsAdded = 0;

            for (const auto& path : System::DroppedFiles)
            {
                if (exists(path))
                {
                    if (is_regular_file(path))
                    {
                        hosts.emplace_back(
                            Host::endpoint_i{ fmt::format("file:{}{}", path.root_name() == "/" ? "" : "/", path.string() ) },
                            Host::description_i{ path.filename().string() }
                        );
                        ++hostsAdded;
                    }
                }
            }

            if (hostsAdded == 1 && m_state == State::Disconnected)
            {
                m_hostSettings.selectedHost = static_cast<uint32_t>(hosts.size() - 1);
                m_selectedHost = &hosts.back();
                m_state = State::Pending;
            }

            System::DroppedFiles.clear();
        }

        ImGui::BeginDisabled(m_state == State::Connecting);
        m_selectedHost = &hosts[m_hostSettings.selectedHost];

        // render hosts list
        {
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::BeginCombo("##Hosts", m_selectedHost->description->data(), ImGuiComboFlags_HeightLarge))
            {
                if (ImGui::Selectable("<New>"))
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
                    auto create_host_label = [this](const Host& host)
                    {
                        m_hostLabel.clear();
                        m_hostLabel += host.description;
                        m_hostLabel += " [";
                        m_hostLabel += host.endpoint;
                        m_hostLabel += "]";
                        return m_hostLabel.data();
                    };

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
            ImGuiExt::TooltipLastHoveredItem(m_selectedHost->endpoint->data());
        }

        // render auto connect option
        {
            ImGui::SameLine();
            ImGui::Checkbox("Auto", &*m_hostSettings.autoConnect);

            if (*m_hostSettings.autoConnect && (m_state == State::Disconnected || (m_state == State::Error && m_deltaSinceError > 5.0f)))
            {
                m_state = State::Pending;
            }

            ImGuiExt::TooltipLastHoveredItem("Automatically connect/reconnect on startup and connection loss.");
        }

        // render connect button
        {
            ImGui::SameLine();

            if (m_state == State::Connected)
            {
                if (ImGui::Button("Disconnect"))
                {
                    *m_hostSettings.autoConnect = false;
                    disconnect();
                }
            }
            else
            {
                if (ImGui::Button("Connect"))
                {
                    m_state = State::Pending;
                }
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
            ImGui::TextColored(stateColor, "%s", stateStr);
        }

        View& selectedView = m_viewSettings.selectedView;

        // process view select key
        if (ImGui::IsKeyPressed(ImGuiKey_Tab, false) && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
        {
            if (selectedView == View::Cache)
            {
                selectedView = View::Trace;
            }
            else if (selectedView == View::Trace)
            {
                selectedView = View::Cache;
            }
        }

        // render view selector
        if (m_cacheView != std::nullopt)
        {
            constexpr std::pair<View, const char*> ViewLabels[] = {
                { View::Cache, "Cache" },
                { View::Trace, "Trace" }
            };

            ImGui::SameLine();
            ImGui::TextUnformatted("View");

            ImGui::SameLine();
            ImGui::PushItemWidth(120.0f);

            if (ImGui::BeginCombo("##Views", ViewLabels[static_cast<int>(selectedView)].second))
            {
                for (const auto& [view, label] : ViewLabels)
                {
                    if (ImGui::Selectable(label, view == selectedView) && view != selectedView)
                    {
                        selectedView = view;
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::PopItemWidth();
        }

        // render help area
        {
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - m_helpHintWidth);

            if (m_releaseInfoTask != std::nullopt)
            {
                if (auto status = m_releaseInfoTask->wait_for(std::chrono::milliseconds{ 0 }); status == std::future_status::ready)
                {
                    try
                    {
                        m_releaseInfo.emplace(m_releaseInfoTask->get());
                    }
                    catch (...)
                    {
                    }

                    m_releaseInfoTask = std::nullopt;
                }
            }

            const char* label;
            int colorsPushed = 1;

            if (m_releaseInfo != std::nullopt && m_releaseInfo->tag_name.isValid() && *m_releaseInfo->tag_name > Version::CurrentVersion)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
                label = "Update Available [F1]";
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.Disabled);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_Button]);
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
                label = "Help/About [F1]";
                colorsPushed = 3;
            }

            if (ImGui::Button(label) || ImGui::IsKeyPressed(ImGuiKey_F1, false))
            {
                m_helpDialog.emplace(m_releaseInfo == std::nullopt ? nullptr : &*m_releaseInfo);
            }

            ImGui::PopStyleColor(colorsPushed);
            m_helpHintWidth = ImGui::GetItemRectSize().x;

            if (m_helpDialog != std::nullopt && !m_helpDialog->render())
            {
                m_helpDialog = std::nullopt;
            }
        }

        ImGui::Separator();

        // render views
        if (m_cacheView != std::nullopt)
        {
            if (selectedView == View::Cache)
            {
                m_cacheView->render();
            }
            else if (selectedView == View::Trace)
            {
                m_traceView->render();
            }
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

void HostView::disconnect()
{
    boost::asio::io_context& ioContext = dots::io::global_io_context();

    try
    {
        ioContext.stop();
        m_cacheView.reset();
        m_traceView.reset();
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

void HostView::update()
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
                    dots::type::Registry::StaticTypePolicy::InternalOnly,
                    transition_handler_t{ &HostView::handleTransceiverTransition, this }
                );

                dots::io::Endpoint endpoint{ *m_selectedHost->endpoint };

                if (endpoint.scheme() == "file" || endpoint.scheme() == "file-v1")
                {
                    std::string_view path = endpoint.path();
                    #ifdef _WIN32
                    if (path.front() == '/')
                    {
                        path.remove_prefix(1);
                    }
                    #endif
                    transceiver.open<dots::io::FileInChannel>(path);
                }
                else
                {
                    transceiver.open(endpoint);
                }

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
                    m_cacheView.emplace();
                    m_traceView.emplace();
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
            {
                using namespace dots::literals;
                auto start = dots::steady_timepoint_t::Now();

                for (size_t totalHandlersExecuted = 0;;)
                {
                    size_t handlersExecuted = dots::global_transceiver()->ioContext().poll_one();
                    totalHandlersExecuted += handlersExecuted;

                    if (!handlersExecuted || (totalHandlersExecuted % 1000 == 0 && (dots::steady_timepoint_t::Now() - start > 15ms)))
                    {
                        break;
                    }
                }
            }
            break;
        case State::Error:
            m_deltaSinceError += ImGui::GetIO().DeltaTime;
            break;
    }
}

void HostView::handleTransceiverTransition(const dots::Connection& connection, std::exception_ptr ePtr)
{
    m_connectionError = ePtr;

    if (connection.closed())
    {
        m_deltaSinceError = 0.0f;
        m_state = State::Error;
    }
}
