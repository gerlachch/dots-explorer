#pragma once
#include <string_view>
#include <future>
#include <GitHubReleaseInfo.dots.h>

struct Version
{
    static constexpr std::string_view CurrentVersion = "v0.9.1";

    static std::future<GitHubReleaseInfo> GetReleaseInfo(std::string_view release = "latest");
};
