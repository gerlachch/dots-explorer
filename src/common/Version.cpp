#include <common/Version.h>
#include <string>
#include <regex>
#include <cstdio>
#include <fmt/format.h>
#include <dots/serialization/TextSerializer.h>
#include <dots/serialization/formats/JsonReader.h>
#include <dots/serialization/formats/JsonWriter.h>

struct GitHubJsonSerializerFormat : dots::serialization::TextSerializerFormat
{
    using reader_t = dots::serialization::JsonReader;
    using writer_t = dots::serialization::JsonWriter;

    static constexpr PropertySetFormat PropertySetFormat = PropertySetFormat::DecimalValue;
    static constexpr TimepointFormat TimepointFormat = TimepointFormat::ISO8601String;
    static constexpr EnumFormat EnumFormat = EnumFormat::String;
};

struct GitHubJsonSerializer : dots::serialization::TextSerializer<GitHubJsonSerializerFormat, GitHubJsonSerializer>
{
    using base_t = TextSerializer<GitHubJsonSerializerFormat, GitHubJsonSerializer>;
    using data_t = std::string;

    using base_t::base_t;
};

std::future<GitHubReleaseInfo> Version::GetReleaseInfo(std::string_view release/* = "latest"*/)
{
    return std::async(std::launch::async, [=]
    {
        #ifdef _WIN32
        #define popen _popen
        #define pclose _pclose
        constexpr char CurlCmd[] = "curl.exe";
        #else
        constexpr char CurlCmd[] = "curl";
        #endif

        std::string cmd{ fmt::format("{} https://api.github.com/repos/gerlachch/dots-explorer/releases/{}", CurlCmd, release) };

        char line[1024];
        FILE* pipe;

        if (pipe = ::popen(cmd.data(), "rt"); pipe == nullptr)
        {
           throw std::runtime_error{ "error getting release information: could not execute curl" };
        }

        std::string body;

        while(std::fgets(line, sizeof line, pipe))
        {
            body += line;
        }

        if (std::feof(pipe))
        {
            ::pclose(pipe);
        }
        else
        {
            throw std::runtime_error{ "error getting release information: broken pipe" };
        }

        // note that this is currently necessary because the JSON serializer
        // expects timestamps to use the offset format
        body = std::regex_replace(body, std::regex{ "Z\"" }, "+00:00\"");

        return GitHubJsonSerializer::Deserialize<GitHubReleaseInfo>(body);
    });
}
