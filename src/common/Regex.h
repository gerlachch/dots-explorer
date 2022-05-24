#pragma once
#include <string_view>
#include <memory>
#include <re2/re2.h>

struct Regex
{
    Regex(std::string_view regex, bool matchCase = true);
    Regex(const Regex& other) = delete;
    Regex(Regex&& other) = default;
    ~Regex() = default;

    Regex& operator = (const Regex& rhs) = delete;
    Regex& operator = (Regex&& rhs) = default;

    bool search(std::string_view str);

private:

    std::unique_ptr<RE2> m_regex;
};
