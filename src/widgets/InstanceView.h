#pragma once
#include <dots/dots.h>
#include <models/MetadataModel.h>
#include <models/StructModel.h>
#include <widgets/PropertyView.h>

struct ImGuiTableSortSpecs;

struct InstanceView
{
    enum MetaData { LastOp, LastPublished, LastPublishedBy, MetaDataSize };

    InstanceView(const StructDescriptorModel& structDescriptorModel, const dots::type::Struct& instance);
    InstanceView(const InstanceView& other) = delete;
    InstanceView(InstanceView&& other) = default;
    ~InstanceView() = default;

    InstanceView& operator = (const InstanceView& rhs) = delete;
    InstanceView& operator = (InstanceView&& rhs) = default;

    const char* widgetId() const;

    const MetadataModel& metadataModel() const;
    MetadataModel& metadataModel();

    const StructModel& structModel() const ;
    StructModel& structModel();

    bool less(const ImGuiTableSortSpecs& sortSpecs, const InstanceView& other) const;

    bool isSelected() const;
    void render();

private:

    inline static uint64_t M_nextWidgetId = 0;

    mutable std::string m_widgetId;
    MetadataModel m_metadataModel;
    StructModel m_structModel;
    std::vector<PropertyView> m_propertyViews;
};
