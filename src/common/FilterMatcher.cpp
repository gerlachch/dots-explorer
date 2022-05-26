#include <common/FilterMatcher.h>
#include <fmt/format.h>

FilterMatcher::FilterMatcher(std::string_view expression, bool matchCase/* = true*/) :
    m_regex{ std::make_unique<re2::RE2>(fmt::format("{}{}", matchCase ? "" : "(?i)", expression), RE2::Options{ RE2::Quiet })}
{
    if (!m_regex->ok())
    {
        throw std::runtime_error{ "invalid regex input: " + m_regex->error() };
    };
}

bool FilterMatcher::match(std::string_view str) const
{
    return RE2::PartialMatch(str, *m_regex);
}
