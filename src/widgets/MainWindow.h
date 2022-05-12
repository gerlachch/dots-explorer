#pragma once
#include <string>
#include <widgets/views/HostView.h>

struct MainWindow
{
    MainWindow(std::string appName);
    void render();

private:

    HostView m_hostView;
};
