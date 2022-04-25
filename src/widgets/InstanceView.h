#pragma once
#include <dots/dots.h>
#include <widgets/StructDescription.h>
#include <widgets/PropertyView.h>

struct ImGuiTableSortSpecs;

struct InstanceView
{
    enum MetaData { LastOp, LastPublished, LastPublishedBy, MetaDataSize };

    InstanceView(const dots::type::Struct& instance);
    InstanceView(const InstanceView& other) = delete;
    InstanceView(InstanceView&& other) = default;
    ~InstanceView() = default;

    InstanceView& operator = (const InstanceView& rhs) = delete;
    InstanceView& operator = (InstanceView&& rhs) = default;

    const char* widgetId() const;
    DotsMt lastOperation() const;
    const dots::type::Struct& instance() const;

    bool less(const ImGuiTableSortSpecs& sortSpecs, const InstanceView& other) const;
    bool isSelected() const;

    void update(const dots::Event<>& event);
    void render(const StructDescription& structDescription, const std::vector<PropertyDescription>& propertyDescriptions);

private:

    using struct_ref_t = std::reference_wrapper<const dots::type::Struct>;

    inline static uint64_t M_nextWidgetId = 0;

    dots::timepoint_t m_lastPublished;
    dots::uint32_t m_lastPublishedFrom;
    std::string m_widgetId;
    std::vector<std::string> m_metaDataStrs;
    DotsMt m_lastOperation;
    struct_ref_t m_instance;
    std::vector<PropertyView> m_propertyViews;
};
