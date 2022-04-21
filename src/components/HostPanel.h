#pragma once
#include <string>
#include <future>
#include <widgets/PoolView.h>
#include <dots/dots.h>

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

    std::optional<PoolView> m_poolView;
    std::optional<std::future<void>> m_connectTask;
    std::exception_ptr m_connectionError;
    State m_state;
    bool m_autoConnect;
    std::string m_endpointBuffer;
    std::string m_appName;
};
