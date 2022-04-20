#if defined(_MSC_VER) && defined (NDEBUG)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#include <backends/GlfwBackend.h>
#include <fmt/format.h>
#include <components/MainWindow.h>
#include <common/Settings.h>

int main()
{
    std::string appName = "dots-explorer";
    std::string appTitle = fmt::format("{}", appName);
    std::string appConfig = fmt::format("{}.ini", appName);
    std::string appLog = fmt::format("{}.log", appName);

    GlfwBackend backend{ 1280, 720, appTitle };
    ImGui::GetIO().IniFilename = appConfig.data();
    ImGui::GetIO().LogFilename = appLog.data();

    Settings::Init();
    MainWindow mainWindow{ appName };

    backend.run([&]
    {
        mainWindow.render();
    });
}
