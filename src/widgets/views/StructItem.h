#pragma once
#include <dots/dots.h>
#include <models/MetadataModel.h>
#include <models/StructModel.h>

struct ImGuiTableSortSpecs;

struct StructItem
{
    enum MetaData { LastOp, LastPublished, LastPublishedBy, MetaDataSize };

    StructItem(const StructDescriptorModel& structDescriptorModel, const PublisherModel& publisherModel, const dots::type::Struct& instance);
    StructItem(const StructItem& other) = delete;
    StructItem(StructItem&& other) = default;
    ~StructItem() = default;

    StructItem& operator = (const StructItem& rhs) = delete;
    StructItem& operator = (StructItem&& rhs) = default;

    const char* widgetId() const;

    const MetadataModel& metadataModel() const;
    MetadataModel& metadataModel();

    const StructModel& structModel() const ;
    StructModel& structModel();

    bool less(const ImGuiTableSortSpecs& sortSpecs, const StructItem& other) const;

    bool isSelected() const;
    bool isHovered() const;

    void render(bool hoverCondition);

private:

    using property_model_ref_t = std::reference_wrapper<PropertyModel>;

    inline static uint64_t M_nextWidgetId = 0;

    mutable std::string m_widgetId;
    bool m_isSelected;
    bool m_isHovered;
    MetadataModel m_metadataModel;
    StructModel m_structModel;
    std::vector<property_model_ref_t> m_propertyModels;
};
