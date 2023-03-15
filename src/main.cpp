#include <string>
#include <optional>
#include <filesystem>
#include <fstream>
#include <fmt/format.h>
#include <dots/tools/logging.h>
#include <backends/Backend.h>
#include <widgets/MainWindow.h>
#include <common/Settings.h>

inline std::string AppName = "dots-explorer";
using path_t = std::filesystem::path;

path_t init_app_data_dir()
{
    #ifdef _WIN32
    constexpr char HomePathVariable[] = "USERPROFILE";
    #else
    constexpr char HomePathVariable[] = "HOME";
    #endif

    if (const char* homePath = ::getenv(HomePathVariable); homePath == nullptr)
        return std::filesystem::current_path();
    else
    {
        path_t appDataPath = std::filesystem::canonical(homePath) / fmt::format(".{}", AppName);

        if (exists(appDataPath) || create_directory(appDataPath))
            return appDataPath;
        else
            return std::filesystem::current_path();
    }
}

auto init_app_data() -> std::pair<std::string, std::string>
{
    std::string appConfigName = fmt::format("{}.ini", AppName);
    std::string appLogName = fmt::format("{}.log", AppName);

    path_t cwdPath = std::filesystem::current_path();
    path_t appConfigPathCwd = cwdPath / appConfigName;
    path_t appLogPathCwd = cwdPath / appLogName;

    path_t appDataPath = init_app_data_dir();
    path_t appConfigPath = appDataPath / appConfigName;
    path_t appLogPath = appDataPath / appLogName;

    // migrate local config if global is not present
    if (!exists(appConfigPath) && exists(appConfigPathCwd))
    {
        if (!copy_file(appConfigPathCwd, appConfigPath))
        {
            appConfigPath = appConfigPathCwd;
            appLogPath = appLogPathCwd;
        }
    }

    return { appConfigPath.string(), appLogPath.string() };
}

int main()
{
    try
    {
        std::optional<Backend> backend{ std::in_place, 1600, 900, AppName };

        auto [appConfigPath, appLogPath] = init_app_data();
        ImGui::GetIO().IniFilename = appConfigPath.data();
        ImGui::GetIO().LogFilename = appLogPath.data();

        dots::type::chrono::experimental::set_time_zone_override();

        #ifdef NDEBUG
        dots::tools::loggingFrontend().setLogLevel(dots::tools::Level::crit);
        #endif

        Settings::Init();
        MainWindow mainWindow{ AppName };

        backend->run([&]
        {
            mainWindow.render();
        });

        backend.reset();
        Settings::Clear();
    }
    catch (const std::exception& e)
    {
        std::ofstream ofstream{ "dots-explorer.crash", std::ios_base::app };
        ofstream << fmt::format("[{}] ERROR running dots-explorer -> '{}'\n", dots::timepoint_t::Now().toString(), e.what());
    }
    catch (...)
    {
        std::ofstream ofstream{ "dots-explorer.crash", std::ios_base::app };
        ofstream << fmt::format("[{}] ERROR running dots-explorer -> '<unknown-exception>'\n", dots::timepoint_t::Now().toString());
    }
}
