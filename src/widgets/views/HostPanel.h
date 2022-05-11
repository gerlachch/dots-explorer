#pragma once
#include <string>
#include <future>
#include <dots/dots.h>
#include <widgets/dialogs/HelpDialog.h>
#include <widgets/dialogs/HostSettingsEdit.h>
#include <widgets/views/CacheView.h>
#include <widgets/views/TraceView.h>
#include <HostSettings.dots.h>
#include <ViewSettings.dots.h>

struct HostPanel
{
    HostPanel(std::string appName);
    HostPanel(const HostPanel& other) = delete;
    HostPanel(HostPanel&& other) = delete;
    ~HostPanel();

    HostPanel& operator = (const HostPanel& rhs) = delete;
    HostPanel& operator = (HostPanel&& rhs) = delete;

    void render();

private:

    enum struct State : uint8_t
    {
        Disconnected,
        Pending,
        Connecting,
        Connected,
        Error
    };

    void disconnect();
    void update();
    void handleTransceiverTransition(const dots::Connection& connection, std::exception_ptr ePtr);

    std::optional<HelpDialog> m_helpDialog;
    std::optional<HostSettingsEdit> m_hostSettingsEdit;
    std::optional<CacheView> m_cacheView;
    std::optional<TraceView> m_traceView;
    std::optional<std::future<void>> m_connectTask;
    std::exception_ptr m_connectionError;
    State m_state;
    Host* m_selectedHost;
    float m_deltaSinceError;
    float m_helpHintWidth;
    HostSettings& m_hostSettings;
    ViewSettings& m_viewSettings;
    std::string m_hostLabel;
    std::string m_appName;
};