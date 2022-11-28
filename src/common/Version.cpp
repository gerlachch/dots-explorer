#include <common/Version.h>
#include <boost/process.hpp>
#ifdef _WIN32
#include <boost/process/windows.hpp>
#endif
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
        constexpr std::string_view ReleasesUri = "https://api.github.com/repos/gerlachch/dots-explorer/releases";

        #ifdef NDEBUG
        constexpr std::string_view Args = "--silent";
        #else
        constexpr std::string_view Args = "";
        #endif

        boost::process::ipstream pipeStream;
        auto redirectStdout = boost::process::std_out > pipeStream;

        #ifdef _WIN32
        boost::process::child curl{ fmt::format("curl.exe {} {}/{}", Args, ReleasesUri, release), redirectStdout, ::boost::process::windows::create_no_window };
        #else
        boost::process::child curl{ fmt::format("curl {} {}/{}", Args, ReleasesUri, release), redirectStdout };
        #endif

        std::string body;
        std::string line;

        while (pipeStream && std::getline(pipeStream, line) && !line.empty())
        {
            body += line;
        }

        curl.wait();

        return GitHubJsonSerializer::Deserialize<GitHubReleaseInfo>(body);
    });
}
