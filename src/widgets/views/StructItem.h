#pragma once
#include <optional>
#include <dots/dots.h>
#include <models/EventModel.h>

struct ImGuiTableSortSpecs;

struct StructItem
{
    enum MetaData { LastOp, LastPublished, LastPublishedBy, MetaDataSize };

    StructItem(EventModel model);

    const EventModel& model() const;
    void setModel(EventModel model);

    bool less(const ImGuiTableSortSpecs& sortSpecs, const StructItem& other) const;

    bool isSelected() const;
    bool isHovered() const;

    void render(bool hoverCondition);
    void renderTooltip() const;

private:

    using property_model_ref_t = std::reference_wrapper<const PropertyModel>;

    std::optional<EventModel> m_model;
    bool m_isSelected;
    bool m_isHovered;
    std::vector<property_model_ref_t> m_propertyModels;
};
