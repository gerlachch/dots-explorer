#pragma once
#include <string>
#include <future>
#include <dots/dots.h>
#include <common/Version.h>
#include <models/TransceiverModel.h>
#include <widgets/dialogs/FileDialog.h>
#include <widgets/dialogs/HelpDialog.h>
#include <widgets/dialogs/HostSettingsEdit.h>
#include <widgets/views/CacheView.h>
#include <widgets/views/TraceView.h>
#include <HostSettings.dots.h>
#include <ViewSettings.dots.h>

struct HostView
{
    HostView(std::string appName);
    HostView(const HostView& other) = delete;
    HostView(HostView&& other) = delete;
    ~HostView();

    HostView& operator = (const HostView& rhs) = delete;
    HostView& operator = (HostView&& rhs) = delete;

    void render();

private:

    enum struct State : uint8_t
    {
        Disconnected,
        Pending,
        Connecting,
        Connected,
        Error
    };

    void disconnect();
    void update();
    void handleTransceiverTransition(const dots::Connection& connection, std::exception_ptr ePtr);

    std::optional<HelpDialog> m_helpDialog;
    std::optional<FileSaveDialog> m_fileSaveDialog;
    std::optional<HostSettingsEdit> m_hostSettingsEdit;
    std::optional<TransceiverModel> m_transceiverModel;
    std::optional<CacheView> m_cacheView;
    std::optional<TraceView> m_traceView;
    std::optional<std::future<void>> m_connectTask;
    std::exception_ptr m_connectionError;
    State m_state;
    Host* m_selectedHost;
    float m_deltaSinceError;
    float m_helpHintWidth;
    HostSettings& m_hostSettings;
    ViewSettings& m_viewSettings;
    std::optional<std::future<GitHubReleaseInfo>> m_releaseInfoTask;
    std::optional<GitHubReleaseInfo> m_releaseInfo;
    std::string m_hostLabel;
    std::string m_appName;
};
