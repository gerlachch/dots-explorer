#include <widgets/input/FilterExpressionEdit.h>
#include <imgui.h>
#include <fmt/format.h>
#include <common/FilterMatcher.h>
#include <common/Colors.h>

FilterExpressionEdit::FilterExpressionEdit(std::string_view expression/* = {}*/, std::string hint/* = "<none>"*/) :
    m_label{ fmt::format("##FilterExpressionEdit-{}", ++M_id) },
    m_isValid(true),
    m_buffer(256, '\0'),
    m_hint{ std::move(hint) }
{
    if (!expression.empty())
    {
        m_buffer.assign(std::max(expression.size(), m_buffer.size()), '\0');
        std::copy(expression.begin(), expression.end(), m_buffer.begin());
    }

    fetch(ImGui::GetStyleColorVec4(ImGuiCol_Text));
}

const ImGuiExt::ColoredText& FilterExpressionEdit::text() const
{
    return m_text;
}

bool FilterExpressionEdit::isValid() const
{
    return m_isValid;
}

bool FilterExpressionEdit::render()
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

void FilterExpressionEdit::fetch(const ImVec4& regularTextColor)
{
    try
    {
        FilterMatcher filterMatcher{ m_buffer.data() };
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
