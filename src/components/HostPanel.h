#pragma once
#include <string>
#include <future>
#include <widgets/PoolView.h>
#include <dots/Application.h>

struct HostPanel
{
    HostPanel(std::string appName, int argc, char** argv);
    void render();

private:

    enum struct State : uint8_t
    {
        Pending,
        Connecting,
        Connected,
        Error
    };

    void update();

    std::optional<dots::Application> m_application;
    std::optional<PoolView> m_poolView;
    std::optional<std::future<void>> m_connectTask;
    State m_state;
    bool m_autoReconnect;
    std::string m_appName;
    int m_argc;
    char** m_argv;
};
