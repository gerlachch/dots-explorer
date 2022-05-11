#pragma once
#include <string>
#include <widgets/views/HostPanel.h>

struct MainWindow
{
    MainWindow(std::string appName);
    void render();

private:

    HostPanel m_hostPanel;
};
