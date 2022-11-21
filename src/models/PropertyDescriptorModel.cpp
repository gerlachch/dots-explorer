#include <models/PropertyDescriptorModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

PropertyDescriptorModel::PropertyDescriptorModel(const dots::type::PropertyPath& path) :
    m_data(std::make_shared<data>(data{
        .propertyPath = path
    }))
{
    const dots::type::PropertyDescriptor& descriptor = path.destination();

    // create description parts
    {
        m_data->declarationText.emplace_back(
            fmt::format("{: >{}}{: >2}:", "", 2 * (path.elements().size() - 1), descriptor.tag()),
            ColorThemeActive.IntegralType
        );

        if (descriptor.isKey())
        {
            m_data->declarationText.emplace_back("[key]", ColorThemeActive.Keyword);
        }

        m_data->declarationText.emplace_back(
            descriptor.valueDescriptor().name(),
            descriptor.valueDescriptor().isFundamentalType() ? ColorThemeActive.Keyword : ColorThemeActive.UserType
        );

        m_data->declarationText.emplace_back(descriptor.name(), ImGui::GetStyle().Colors[ImGuiCol_Text]);
    }

    // set value color
    switch (descriptor.valueDescriptor().type())
    {
        case dots::type::Type::boolean:
            m_data->valueColor = ColorThemeActive.Keyword;
            break;
        case dots::type::Type::string:
            m_data->valueColor = ColorThemeActive.StringType;
            break;
        case dots::type::Type::Enum:
            m_data->valueColor = ColorThemeActive.EnumType;
            break;
        case dots::type::Type::int8:
        case dots::type::Type::uint8:
        case dots::type::Type::int16:
        case dots::type::Type::uint16:
        case dots::type::Type::int32:
        case dots::type::Type::uint32:
        case dots::type::Type::int64:
        case dots::type::Type::uint64:
        case dots::type::Type::float32:
        case dots::type::Type::float64:
        case dots::type::Type::property_set:
        case dots::type::Type::timepoint:
        case dots::type::Type::steady_timepoint:
        case dots::type::Type::duration:
        case dots::type::Type::uuid:
        case dots::type::Type::Vector:
        case dots::type::Type::Struct:
        default:
            m_data->valueColor = ColorThemeActive.IntegralType;
            break;
    }
}

const dots::type::PropertyPath& PropertyDescriptorModel::propertyPath() const
{
    return m_data->propertyPath;
}

const std::vector<ImGuiExt::ColoredText>& PropertyDescriptorModel::declarationText() const
{
    return m_data->declarationText;
}

const ImVec4& PropertyDescriptorModel::valueColor() const
{
    return m_data->valueColor;
}
