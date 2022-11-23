#pragma once
#include <optional>
#include <unordered_map>
#include <vector>
#include <dots/dots.h>
#include <common/FilterMatcher.h>
#include <models/EventModel.h>
#include <widgets/views/StructItem.h>
#include <widgets/dialogs/PublishDialog.h>
#include <FilterSettings.dots.h>

struct ImGuiTableSortSpecs;

struct StructList
{
    StructList(const StructDescriptorModel& descriptorModel);
    StructList(const StructList& other) = delete;
    StructList(StructList&& other) = delete;
    ~StructList() = default;

    StructList& operator = (const StructList& rhs) = delete;
    StructList& operator = (StructList&& rhs) = delete;

    size_t size() const;
    bool less(const ImGuiTableSortSpecs& sortSpecs, const StructList& other) const;
    bool isFiltered(const std::optional<FilterMatcher>& filter, const FilterSettings& filterSettings) const;

    void update(const EventModel& eventModel);
    bool renderBegin();
    void renderEnd();
    void renderActivity();
    void renderActivityDot();

private:

    std::optional<PublishDialog> m_publishDialog;
    std::unordered_map<size_t, StructItem> m_itemsStorage;
    std::vector<std::reference_wrapper<StructItem>> m_items;
    std::vector<const StructItem*> m_itemsDirty;
    std::vector<std::string> m_headers;
    std::string m_typeNameLower;
    const StructItem* m_lastPublishedItem;
    dots::timepoint_t m_lastPublishedItemTime;
    float m_lastUpdateDelta;
    bool m_containerChanged;
    const StructDescriptorModel* m_structDescriptorModel;
};
