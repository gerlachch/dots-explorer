#pragma once
#include <vector>
#include <dots/dots.h>
#include <widgets/StructList.h>
#include <FilterSettings.dots.h>

struct TypeList
{
    TypeList();
    TypeList(const TypeList& other) = delete;
    TypeList(TypeList&& other) = delete;
    ~TypeList() = default;

    TypeList& operator = (const TypeList& rhs) = delete;
    TypeList& operator = (TypeList&& rhs) = delete;

    void update(const dots::type::StructDescriptor<>& descriptor);
    void render();

private:

    std::vector<std::shared_ptr<StructList>> m_structLists;
    std::vector<std::shared_ptr<StructList>> m_structListsFiltered;
    std::string m_containerFilterBuffer;
    bool m_typesChanged;
    bool m_filterSettingsInitialized;
    FilterSettings& m_filterSettings;
    std::vector<dots::Subscription> m_subscriptions;
};
