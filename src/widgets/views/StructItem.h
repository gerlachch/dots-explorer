#pragma once
#include <dots/dots.h>
#include <models/EventModel.h>

struct ImGuiTableSortSpecs;

struct StructItem
{
    enum MetaData { LastOp, LastPublished, LastPublishedBy, MetaDataSize };

    StructItem(std::shared_ptr<const EventModel> eventModel);
    StructItem(const StructItem& other) = delete;
    StructItem(StructItem&& other) = default;
    ~StructItem() = default;

    StructItem& operator = (const StructItem& rhs) = delete;
    StructItem& operator = (StructItem&& rhs) = default;

    const char* widgetId() const;
    const EventModel& model() const;
    const std::shared_ptr<const EventModel>& modelPtr() const;

    bool less(const ImGuiTableSortSpecs& sortSpecs, const StructItem& other) const;

    bool isSelected() const;
    bool isHovered() const;

    void render(bool hoverCondition);

private:

    using struct_model_ref_t = std::reference_wrapper<const StructModel>;
    using property_model_ref_t = std::reference_wrapper<const PropertyModel>;

    inline static uint64_t M_nextWidgetId = 0;

    mutable std::string m_widgetId;
    bool m_isSelected;
    bool m_isHovered;
    std::shared_ptr<const EventModel> m_model;
    std::vector<property_model_ref_t> m_propertyModels;
};
