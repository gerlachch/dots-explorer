#pragma once
#include <string_view>
#include <future>
#include <GitHubReleaseInfo.dots.h>

struct Version
{
    static constexpr std::string_view CurrentVersion = "v1.0.0";

    static std::future<GitHubReleaseInfo> GetReleaseInfo(std::string_view release = "latest");
};
