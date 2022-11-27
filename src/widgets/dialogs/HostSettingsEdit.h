#pragma once
#include <dots/dots.h>
#include <widgets/dialogs/FileDialog.h>
#include <HostSettings.dots.h>

struct HostSettingsEdit
{
    HostSettingsEdit(HostSettings& settings, const Host& copyHost, Host* editHost);
    HostSettingsEdit(const HostSettingsEdit& other) = delete;
    HostSettingsEdit(HostSettingsEdit&& other) = delete;
    ~HostSettingsEdit() = default;

    HostSettingsEdit& operator = (const HostSettingsEdit& rhs) = delete;
    HostSettingsEdit& operator = (HostSettingsEdit&& rhs) = delete;
    
    bool render();

private:

    inline static uint64_t M_id;

    std::optional<FileOpenDialog> m_fileOpenDialog;
    std::string m_endpointBuffer;
    std::string m_descriptionBuffer;
    std::string m_popupId;
    const char* m_headerText;
    HostSettings& m_settings;
    Host* m_editHost;
};
