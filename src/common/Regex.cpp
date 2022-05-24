#include <common/Regex.h>
#include <fmt/format.h>

Regex::Regex(std::string_view regex, bool matchCase/* = true*/) :
    m_regex{ std::make_unique<re2::RE2>(fmt::format("{}{}", matchCase ? "" : "(?i)", regex), RE2::Options{ RE2::Quiet })}
{
    if (!m_regex->ok())
    {
        throw std::runtime_error{ "invalid regex input: " + m_regex->error() };
    };
}

bool Regex::search(std::string_view str)
{
    return RE2::PartialMatch(str, *m_regex);
}
