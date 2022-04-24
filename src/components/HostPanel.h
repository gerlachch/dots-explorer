#pragma once
#include <string>
#include <future>
#include <dots/dots.h>
#include <widgets/HostSettingsEdit.h>
#include <widgets/PoolView.h>
#include <HostSettings.dots.h>

struct HostPanel
{
    HostPanel(std::string appName);
    HostPanel(const HostPanel& other) = delete;
    HostPanel(HostPanel&& other) = delete;
    ~HostPanel();

    HostPanel& operator =(const HostPanel& rhs) = delete;
    HostPanel& operator =(HostPanel&& rhs) = delete;

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

    std::optional<HostSettingsEdit> m_hostSettingsEdit;
    std::optional<PoolView> m_poolView;
    std::optional<std::future<void>> m_connectTask;
    std::exception_ptr m_connectionError;
    State m_state;
    Host* m_selectedHost;
    float m_deltaSinceError;
    HostSettings& m_hostSettings;
    std::string m_hostLabel;
    std::string m_appName;
};
