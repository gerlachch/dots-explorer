#pragma once
#include <optional>
#include <unordered_map>
#include <vector>
#include <dots/dots.h>
#include <common/FilterMatcher.h>
#include <models/StructDescriptorModel.h>
#include <models/PublisherModel.h>
#include <widgets/views/StructItem.h>
#include <widgets/dialogs/PublishDialog.h>
#include <FilterSettings.dots.h>

struct ImGuiTableSortSpecs;

struct StructList
{
    StructList(const dots::type::StructDescriptor& descriptor, const PublisherModel& publisherModel);
    StructList(const StructList& other) = delete;
    StructList(StructList&& other) = delete;
    ~StructList() = default;

    StructList& operator = (const StructList& rhs) = delete;
    StructList& operator = (StructList&& rhs) = delete;

    const dots::Container<>& container() const;
    bool less(const ImGuiTableSortSpecs& sortSpecs, const StructList& other) const;
    bool isFiltered(const std::optional<FilterMatcher>& filter, const FilterSettings& filterSettings) const;

    void update(const dots::Event<>& event);
    bool renderBegin();
    void renderEnd();
    void renderActivity();
    void renderActivityDot();

private:

    using container_ref_t = std::reference_wrapper<const dots::Container<>>;
    using publisher_model_ref_t = std::reference_wrapper<const PublisherModel>;

    std::optional<PublishDialog> m_publishDialog;
    std::unordered_map<const dots::type::Struct*, StructItem> m_itemsStorage;
    std::vector<std::reference_wrapper<StructItem>> m_items;
    std::vector<std::string> m_headers;
    std::string m_typeNameLower;
    const StructItem* m_lastPublishedItem;
    dots::timepoint_t m_lastPublishedItemTime;
    float m_lastUpdateDelta;
    bool m_containerChanged;
    std::optional<dots::Container<>> m_containerStorage;
    container_ref_t m_container;
    StructDescriptorModel m_structDescriptorModel;
    publisher_model_ref_t m_publisherModel;
};
