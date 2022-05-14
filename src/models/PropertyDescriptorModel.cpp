#include <models/PropertyDescriptorModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

PropertyDescriptorModel::PropertyDescriptorModel(const dots::type::PropertyPath& path) :
    m_propertyPath{ path }
{
    const dots::type::PropertyDescriptor& descriptor = path.destination();

    // create description parts
    {
        m_declarationText.emplace_back(
            fmt::format("{: >{}}{: >2}:", "", 2 * (path.elements().size() - 1), descriptor.tag()),
            ColorThemeActive.IntegralType
        );

        if (descriptor.isKey())
        {
            m_declarationText.emplace_back("[key]", ColorThemeActive.Keyword);
        }

        m_declarationText.emplace_back(
            descriptor.valueDescriptor().name(),
            descriptor.valueDescriptor().isFundamentalType() ? ColorThemeActive.Keyword : ColorThemeActive.UserType
        );

        m_declarationText.emplace_back(descriptor.name(), ImGui::GetStyle().Colors[ImGuiCol_Text]);
    }
}

const dots::type::PropertyPath& PropertyDescriptorModel::propertyPath() const
{
    return m_propertyPath;
}

const std::vector<ImGuiExt::ColoredText>& PropertyDescriptorModel::declarationText() const
{
    return m_declarationText;
}
