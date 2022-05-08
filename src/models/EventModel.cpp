#include <models/EventModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

EventModel::EventModel(size_t index, const StructDescriptorModel& descriptorModel, const dots::Event<>& event) :
    m_index(index),
    m_publishedInstance(event.transmitted()),
    m_structModel{ descriptorModel, *m_publishedInstance }
{
    m_metadataModel.fetch(event);
}

const StructModel& EventModel::structModel() const
{
    return m_structModel;
}

const MetadataModel& EventModel::metadataModel() const
{
    return m_metadataModel;
}

const ImGuiExt::ColoredText& EventModel::indexText() const
{
    if (m_indexText.first.empty())
    {
        m_indexText = { fmt::format("#{}", m_index), ColorThemeActive.Disabled};
    }

    return m_indexText;
}

const ImGuiExt::ColoredText& EventModel::publishedInstanceTextRaw() const
{
    if (m_publishedInstanceText.first.empty())
    {
        m_publishedInstanceText = { dots::to_string(*m_publishedInstance), ColorThemeActive.IntegralType };
    }

    return m_publishedInstanceText;
}

const std::vector<ImGuiExt::ColoredText>& EventModel::publishedInstanceTextSimple() const
{
    if (m_publishedInstanceTextFancy.empty())
    {
        const ImGuiExt::ColoredText& structDeclarationText = m_structModel.descriptorModel().declarationText()[1];
        m_publishedInstanceTextFancy.emplace_back(structDeclarationText);

        for (const PropertyModel& propertyModel : m_structModel.propertyModels())
        {
            if (!propertyModel.property().isValid())
            {
                continue;
            }

            m_publishedInstanceTextFancy.emplace_back(fmt::format(" {}:", propertyModel.descriptorModel().propertyPath().destination().name()), ColorThemeActive.Identifier);
            m_publishedInstanceTextFancy.emplace_back(propertyModel.valueText());
        }
    }

    return m_publishedInstanceTextFancy;
}

const std::vector<ImGuiExt::ColoredText>& EventModel::publishedInstanceTextFancy() const
{
    if (m_publishedInstanceTextFancy.empty())
    {
        const ImGuiExt::ColoredText& structDeclarationText = m_structModel.descriptorModel().declarationText()[1];
        m_publishedInstanceTextFancy.emplace_back(fmt::format("{}{{", structDeclarationText.first), structDeclarationText.second);

        bool first = true;

        for (const PropertyModel& propertyModel : m_structModel.propertyModels())
        {
            if (!propertyModel.property().isValid())
            {
                continue;
            }

            const char* format;

            if (first)
            {
                first = false;
                format = " .{}=";
            }
            else
            {
                format = ", .{}=";
            }

            m_publishedInstanceTextFancy.emplace_back(fmt::format(format, propertyModel.descriptorModel().propertyPath().destination().name()), ColorThemeActive.Identifier);
            m_publishedInstanceTextFancy.emplace_back(propertyModel.valueText());

        }

        m_publishedInstanceTextFancy.emplace_back(" }", structDeclarationText.second);
    }

    return m_publishedInstanceTextFancy;
}

void EventModel::fetch()
{
    /* do nothing */
}
