#include <widgets/input/RegexEdit.h>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Regex.h>
#include <common/Colors.h>

RegexEdit::RegexEdit(std::string_view regex/* = {}*/, std::string hint/* = "<none>"*/) :
    m_label{ fmt::format("##RegexEdit-{}", ++M_id) },
    m_isValid(true),
    m_buffer(256, '\0'),
    m_hint{ std::move(hint) }
{
    if (!regex.empty())
    {
        m_buffer.assign(std::max(regex.size(), m_buffer.size()), '\0');
        std::copy(regex.begin(), regex.end(), m_buffer.begin());
    }

    fetch(ImGui::GetStyleColorVec4(ImGuiCol_Text));
}

const ImGuiExt::ColoredText& RegexEdit::text() const
{
    return m_text;
}

bool RegexEdit::isValid() const
{
    return m_isValid;
}

bool RegexEdit::render()
{
    bool textChanged = false;
    ImVec4 regularTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    ImGui::PushStyleColor(ImGuiCol_Text, m_text.second);
    if (ImGui::InputTextWithHint(m_label.data(), m_hint.data(), m_buffer.data(), m_buffer.size()))
    {
        fetch(regularTextColor);
        textChanged = m_isValid;
    }
    ImGui::PopStyleColor();

    return textChanged;
}

void RegexEdit::fetch(const ImVec4& regularTextColor)
{
    try
    {
        Regex regex{ m_buffer.data() };
        m_text.first = m_buffer.data();
        m_text.second = regularTextColor;
        m_isValid = true;
    }
    catch (...)
    {
        m_text.second = ColorThemeActive.Error;
        m_isValid = false;
    }
}
