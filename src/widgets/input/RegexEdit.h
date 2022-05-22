#pragma once
#include <common/ImGuiExt.h>

struct RegexEdit
{
    RegexEdit(std::string_view regex = {}, std::string hint = "<none>");

    const ImGuiExt::ColoredText& text() const;
    bool isValid() const;

    bool render();

private:

    inline static uint64_t M_id = 0;

    void fetch(const ImVec4& regularTextColor);

    ImGuiExt::ColoredText m_text;
    std::string m_label;
    bool m_isValid;
    std::string m_buffer;
    std::string m_hint;
};
