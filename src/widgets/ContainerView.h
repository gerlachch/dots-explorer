#pragma once
#include <optional>
#include <unordered_map>
#include <vector>
#include <dots/dots.h>
#include <widgets/StructDescription.h>
#include <widgets/InstanceView.h>
#include <widgets/InstanceEdit.h>

struct ImGuiTableSortSpecs;

struct ContainerView
{
    ContainerView(const dots::type::StructDescriptor<>& descriptor);
    ContainerView(const ContainerView& other) = delete;
    ContainerView(ContainerView&& other) = delete;
    ~ContainerView() = default;

    ContainerView& operator = (const ContainerView& rhs) = delete;
    ContainerView& operator = (ContainerView&& rhs) = delete;

    const dots::Container<>& container() const;
    bool less(const ImGuiTableSortSpecs& sortSpecs, const ContainerView& other) const;

    void update(const dots::Event<>& event);
    bool renderBegin();
    void renderEnd();

private:

    using container_ref_t = std::reference_wrapper<const dots::Container<>>;

    std::optional<InstanceEdit> m_instanceEdit;
    std::unordered_map<const dots::type::Struct*, InstanceView> m_instanceViewsStorage;
    std::vector<std::reference_wrapper<InstanceView>> m_instanceViews;
    std::vector<std::string> m_headers;
    bool m_containerChanged;
    container_ref_t m_container;
    StructDescription m_structDescription;
};
