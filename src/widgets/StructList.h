#pragma once
#include <optional>
#include <unordered_map>
#include <vector>
#include <dots/dots.h>
#include <models/StructDescriptorModel.h>
#include <widgets/StructListRow.h>
#include <widgets/StructEdit.h>

struct ImGuiTableSortSpecs;

struct StructList
{
    StructList(const dots::type::StructDescriptor<>& descriptor);
    StructList(const StructList& other) = delete;
    StructList(StructList&& other) = delete;
    ~StructList() = default;

    StructList& operator = (const StructList& rhs) = delete;
    StructList& operator = (StructList&& rhs) = delete;

    const dots::Container<>& container() const;
    bool less(const ImGuiTableSortSpecs& sortSpecs, const StructList& other) const;

    void update(const dots::Event<>& event);
    bool renderBegin();
    void renderEnd();

private:

    using container_ref_t = std::reference_wrapper<const dots::Container<>>;

    std::optional<StructEdit> m_structEdit;
    std::unordered_map<const dots::type::Struct*, StructListRow> m_rowsStorage;
    std::vector<std::reference_wrapper<StructListRow>> m_rows;
    std::vector<std::string> m_headers;
    bool m_containerChanged;
    std::optional<dots::Container<>> m_containerStorage;
    container_ref_t m_container;
    StructDescriptorModel m_structDescriptorModel;
};
