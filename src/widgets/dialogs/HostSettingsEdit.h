#pragma once
#include <dots/dots.h>
#include <widgets/input/HostEndpointEdit.h>
#include <widgets/dialogs/FileDialog.h>
#include <HostSettings.dots.h>

struct HostSettingsEdit
{
    HostSettingsEdit(HostSettings& settings, Host copyHost, Host* editHost);
    HostSettingsEdit(const HostSettingsEdit& other) = delete;
    HostSettingsEdit(HostSettingsEdit&& other) = delete;
    ~HostSettingsEdit() = default;

    HostSettingsEdit& operator = (const HostSettingsEdit& rhs) = delete;
    HostSettingsEdit& operator = (HostSettingsEdit&& rhs) = delete;
    
    bool render();

private:

    static constexpr uint32_t NoHostSelected = std::numeric_limits<uint32_t>::max();

    inline static uint64_t M_id;

    std::optional<FileOpenDialog> m_fileOpenDialog;
    std::string m_descriptionBuffer;
    std::string m_popupId;
    const char* m_headerText;
    HostSettings& m_settings;
    Host m_host;
    HostEndpointEdit m_endpointEdit;
    Host* m_editHost;
};
