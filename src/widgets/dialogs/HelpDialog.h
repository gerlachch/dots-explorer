#pragma once
#include <dots/dots.h>
#include <GitHubReleaseInfo.dots.h>

struct HelpDialog
{
    HelpDialog(const GitHubReleaseInfo* releaseInfo = nullptr);
    HelpDialog(const HelpDialog& other) = delete;
    HelpDialog(HelpDialog&& other) = delete;
    ~HelpDialog() = default;

    HelpDialog& operator = (const HelpDialog& rhs) = delete;
    HelpDialog& operator = (HelpDialog&& rhs) = delete;

    bool render();

private:

    inline static uint64_t M_id;

    std::string m_popupId;
    const GitHubReleaseInfo* m_releaseInfo;
};
