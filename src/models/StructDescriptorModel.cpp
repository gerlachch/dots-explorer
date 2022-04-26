#include <models/StructDescriptorModel.h>
#include <common/Colors.h>

StructDescriptorModel::StructDescriptorModel(const dots::type::StructDescriptor<>& descriptor) :
    m_descriptor{ descriptor }
{
    m_declarationText.emplace_back("struct", ColorThemeActive.Keyword);
    m_declarationText.emplace_back(descriptor.name(), ColorThemeActive.UserType);

    if (uint8_t flags = descriptor.flags(); flags != dots::type::StructDescriptor<>::Cached)
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

        m_declarationText.emplace_back(std::move(part), ColorThemeActive.Keyword);
    }

    for (const dots::type::PropertyPath& propertyPath : m_descriptor.get().propertyPaths())
    {
        m_propertyDescriptorModels.emplace_back(propertyPath);
    }
}

const std::vector<PropertyDescriptorModel>& StructDescriptorModel::propertyDescriptorModels() const
{
    return m_propertyDescriptorModels;
}

const dots::type::StructDescriptor<>& StructDescriptorModel::descriptor() const
{
    return m_descriptor;
}

auto StructDescriptorModel::declarationText() const -> const std::vector<colored_text_t>&
{
    return m_declarationText;
}

void StructDescriptorModel::fetch()
{
    /* do nothing */
}

void StructDescriptorModel::refresh()
{
    /* do nothing */
}
