#include <backends/GlfwBackend.h>
#include <components/MainWindow.h>
#include <boost/asio.hpp>
#include <fmt/format.h>
#include <dots/Application.h>

int main(int argc, char* argv[])
{
    std::string appName = "dots-explorer";
    std::string appTitle = fmt::format("{}", appName);
    std::string appConfig = fmt::format("{}.ini", appName);
    std::string appLog = fmt::format("{}.log", appName);

    dots::Application app{ "dots-explorer", argc, argv };

    GlfwBackend backend{ 1280, 720, appTitle };
    ImGui::GetIO().IniFilename = appConfig.data();
    ImGui::GetIO().LogFilename = appLog.data();

    MainWindow mainWindow;

    dots::publish(DotsDescriptorRequest{});

    backend.run([&]
    {
        app.transceiver().ioContext().poll();
        mainWindow.render();
    });
}
