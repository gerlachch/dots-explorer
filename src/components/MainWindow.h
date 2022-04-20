#pragma once
#include <string>
#include <components/HostPanel.h>

struct MainWindow
{
    MainWindow(std::string appName);
    void render();

private:

    std::optional<HostPanel> m_hostPanel;
    std::string m_endpointBuffer;
    std::string m_appName;
};
