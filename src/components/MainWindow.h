#pragma once
#include <string>
#include <components/HostPanel.h>

struct MainWindow
{
    MainWindow(std::string appName);
    void render();

private:

    HostPanel m_hostPanel;
};
