#include <common/Settings.h>
#include <imgui_internal.h>
#include <dots/serialization/StringSerializer.h>
#include <dots/tools/string_tools.h>

void Settings::Init()
{
    ImGuiSettingsHandler iniHandler;
    iniHandler.TypeName = "UserSettings";
    iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
    iniHandler.ClearAllFn = ClearAllHandler;
    iniHandler.ReadOpenFn = ReadOpenHandler;
    iniHandler.ReadLineFn = ReadLineHandler;
    iniHandler.ApplyAllFn = ApplyAllHandler;
    iniHandler.WriteAllFn = WriteAllHandler;
    ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);
}

dots::type::Struct& Settings::Register(const dots::type::StructDescriptor<>& descriptor)
{
    auto [it, emplaced] = M_settingsMap.try_emplace(descriptor.name(), descriptor);
    return it->second;
}

void Settings::ClearAllHandler(ImGuiContext*/* ctx*/, ImGuiSettingsHandler*/* handler*/)
{
    M_settingsMap.clear();
}

void* Settings::ReadOpenHandler(ImGuiContext*/* ctx*/, ImGuiSettingsHandler*/* handler*/, const char*/* name*/)
{
    return &M_settingsMap;
}

void Settings::ReadLineHandler(ImGuiContext*/* ctx*/, ImGuiSettingsHandler*/* handler*/, void*/* entry*/, const char* line)
{
    try
    {
        auto [name, value] = dots::tools::split_left_at_first_of(line, "=");

        if (auto it = M_settingsMap.find(std::string{ name }); it != M_settingsMap.end())
        {
            dots::from_string(std::string{ value }, *it->second);
        }
    }
    catch (...)
    {
    }
}

void Settings::ApplyAllHandler(ImGuiContext*/* ctx*/, ImGuiSettingsHandler*/* handler*/)
{
    /* do nothing */
}

void Settings::WriteAllHandler(ImGuiContext*/* ctx*/, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
{
    buf->reserve(buf->size() + static_cast<int>(M_settingsMap.size()) * 30);
    buf->appendf("[%s][%s]\n", handler->TypeName, "Global");

    for (const auto& [name, settings] : M_settingsMap)
    {
        std::string value = dots::to_string(*settings);
        buf->appendf("%s=%s\n", name.data(), value.data());
    }

    buf->append("\n");
}
