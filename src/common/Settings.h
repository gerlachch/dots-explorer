#pragma once
#include <string>
#include <map>
#include <imgui.h>
#include <dots/type/AnyStruct.h>

struct ImGuiSettingsHandler;

struct Settings
{
    static void Init();
    static void Clear();

    static dots::type::Struct& Register(const dots::type::StructDescriptor& descriptor);

    template <typename T, std::enable_if_t<std::is_base_of_v<dots::type::Struct, T>, int> = 0>
    static T& Register()
    {
        return Register(T::_Descriptor()).template _to<T>();
    }

private:

    inline static std::map<std::string, dots::type::AnyStruct> M_settingsMap;

    static void ClearAllHandler(ImGuiContext* ctx, ImGuiSettingsHandler*);
    static void* ReadOpenHandler(ImGuiContext*, ImGuiSettingsHandler*, const char* name);
    static void ReadLineHandler(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);
    static void ApplyAllHandler(ImGuiContext* ctx, ImGuiSettingsHandler*);
    static void WriteAllHandler(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
};
