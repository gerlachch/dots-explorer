#include <widgets/StructDescription.h>
#include <common/Colors.h>

StructDescription::StructDescription(const dots::type::StructDescriptor<>& descriptor) :
    m_descriptor{ descriptor }
{
    m_parts.emplace_back("struct", ColorThemeActive.Keyword);
    m_parts.emplace_back(descriptor.name(), ColorThemeActive.UserType);

    if (uint8_t flags = descriptor.flags(); flags > 1)
    {
        std::string part = "[";

        if (!descriptor.cached())
        {
            part += "cached=false, ";
        }

        if (descriptor.cleanup())
        {
            part += "cleanup, ";
        }

        if (descriptor.local())
        {
            part += "local, ";
        }

        if (descriptor.persistent())
        {
            part += "persistent, ";
        }

        if (descriptor.internal())
        {
            part += "internal, ";
        }

        if (descriptor.substructOnly())
        {
            part += "substruct_only, ";
        }

        part.erase(part.size() - 2);
        part += ']';

        m_parts.emplace_back(std::move(part), ColorThemeActive.Keyword);
    }
}

const dots::type::StructDescriptor<>& StructDescription::descriptor() const
{
    return m_descriptor;
}

void StructDescription::render() const
{
    ImGuiExt::TextColored(m_parts);
}
