#include <widgets/InstanceView.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <DotsClient.dots.h>

InstanceView::InstanceView(const dots::type::Struct& instance) :
    m_widgetId{ fmt::format("InstanceView-{}", M_nextWidgetId++) },
    m_metaDataStrs(MetaDataSize, ""),
    m_lastOperation(DotsMt::create),
    m_instance{ instance }
{
    const auto& propertyPaths = m_instance.get()._descriptor().propertyPaths();

    if (propertyPaths.size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (const dots::type::PropertyPath& propertyPath : propertyPaths)
        {
            const dots::type::PropertyDescriptor& propertyDescriptor = propertyPath.destination();

            if (propertyDescriptor.valueDescriptor().type() == dots::type::Type::Struct)
            {
                continue;
            }

            if (propertyPath.elements().size() == 1)
            {
                m_propertyViews.emplace_back(dots::type::ProxyProperty<>{ const_cast<dots::type::Struct&>(m_instance.get()), propertyDescriptor });
            }
            else
            {
                m_propertyViews.emplace_back(dots::type::ProxyProperty<>{ const_cast<dots::type::Struct&>(m_instance.get()), propertyPath });
            }
        }
    }
    else
    {
        for (auto property : m_instance.get())
        {
            m_propertyViews.emplace_back(property);
        }
    }
}

const char* InstanceView::widgetId() const
{
    return m_widgetId.data();
}

DotsMt InstanceView::lastOperation() const
{
    return m_lastOperation;
}

const dots::type::Struct& InstanceView::instance() const
{
    return m_instance;
}

bool InstanceView::less(const ImGuiTableSortSpecs& sortSpecs, const InstanceView& other) const
{
    for (int i = 0; i < sortSpecs.SpecsCount; ++i)
    {
        const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs.Specs[i];
        ImS16 columnIndex = sortSpec.ColumnIndex;

        if (columnIndex >= MetaDataSize)
        {
            columnIndex -= MetaDataSize;
            const PropertyView& propertyViewThis = m_propertyViews[columnIndex];
            const PropertyView& propertyViewOther = other.m_propertyViews[columnIndex];

            if (propertyViewThis.less(sortSpec, propertyViewOther))
            {
                return true;
            }
            else if (propertyViewOther.less(sortSpec, propertyViewThis))
            {
                return false;
            }
        }
        else
        {
            auto compare = [&sortSpec](const auto& lhs, const auto& rhs)
            {
                if (sortSpec.SortDirection == ImGuiSortDirection_Ascending)
                {
                    return std::less{}(lhs, rhs);
                }
                else
                {
                    return std::greater{}(lhs, rhs);
                }
            };

            const std::string& metaDataStrThis = m_metaDataStrs[columnIndex];
            const std::string& metaDataStrOther = other.m_metaDataStrs[columnIndex];

            if (compare(metaDataStrThis, metaDataStrOther))
            {
                return true;
            }
            else if (compare(metaDataStrOther, metaDataStrThis))
            {
                return false;
            }
        }
    }

    return instance()._less(other.instance(), instance()._keyProperties());
}

bool InstanceView::isSelected() const
{
    return std::any_of(m_propertyViews.begin(), m_propertyViews.end(), [](const PropertyView& propertyView){ return propertyView.isSelected(); });
}

void InstanceView::update(const dots::Event<>& event)
{
    m_lastOperation = event.mt();
    m_lastPublished = event.cloneInfo().modified.valueOrDefault(event.cloneInfo().created);
    m_lastPublishedFrom = *event.cloneInfo().lastUpdateFrom;

    for (std::string& metaDataStr : m_metaDataStrs)
    {
        metaDataStr.clear();
    }

    for (PropertyView& propertyView : m_propertyViews)
    {
        propertyView.update();
    }
}

void InstanceView::render()
{
    if (ImGui::TableNextColumn())
    {
        std::string& lastOpStr = m_metaDataStrs[LastOp];

        if (lastOpStr.empty())
        {
            switch (m_lastOperation)
            {
                case DotsMt::create: lastOpStr = "CREATE"; break;
                case DotsMt::update: lastOpStr = "UPDATE"; break;
                case DotsMt::remove: lastOpStr = "REMOVE"; break;
            }
        }

        switch (m_lastOperation)
        {
            case DotsMt::create: ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.Create); break;
            case DotsMt::update: ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.Update); break;
            case DotsMt::remove: ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.Remove); break;
        }

        ImGui::TextUnformatted(lastOpStr.data());
        ImGui::PopStyleColor();
    }

    if (ImGui::TableNextColumn())
    {
        std::string& lastPublishedStr = m_metaDataStrs[LastPublished];

        if (lastPublishedStr.empty())
        {
            lastPublishedStr = m_lastPublished.toString("%F %T");
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.IntegralType);
        ImGui::TextUnformatted(lastPublishedStr.data());
        ImGui::PopStyleColor();
    }

    if (ImGui::TableNextColumn())
    {
        std::string& lastPublishedByStr = m_metaDataStrs[LastPublishedBy];

        if (lastPublishedByStr.empty())
        {
            if (const auto* client = dots::container<DotsClient>().find(DotsClient::id_i{ m_lastPublishedFrom }); client == nullptr || !client->name.isValid())
            {
                lastPublishedByStr = fmt::format("\"<unknown> [{}]\"", m_lastPublishedFrom);
            }
            else
            {
                lastPublishedByStr = fmt::format("\"{}\"", *client->name);
            }
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.StringType);
        ImGui::TextUnformatted(lastPublishedByStr.data());
        ImGui::PopStyleColor();
    }

    for (PropertyView& propertyView : m_propertyViews)
    {
        ImGui::TableNextColumn();
        propertyView.render();
    }
}
