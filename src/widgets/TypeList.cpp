#include <widgets/TypeList.h>
#include <string_view>
#include <regex>
#include <imgui.h>
#include <widgets/StructList.h>
#include <common/Settings.h>
#include <StructDescriptorData.dots.h>
#include <EnumDescriptorData.dots.h>

TypeList::TypeList() :
    m_typeFilterBuffer(256, '\0'),
    m_typesChanged(false),
    m_filterSettingsInitialized(false),
    m_filterSettings{ Settings::Register<FilterSettings>() }
{
    m_subscriptions.emplace_back(dots::subscribe<StructDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<EnumDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<dots::type::StructDescriptor<>>({ &TypeList::update, this }));
}

void TypeList::update(const dots::type::StructDescriptor<>& descriptor)
{
    if (!descriptor.substructOnly())
    {
        StructList& structList = *m_structLists.emplace_back(std::make_shared<StructList>(descriptor));
        m_typesChanged = true;

        m_subscriptions.emplace_back(dots::subscribe(descriptor, [this, &structList](const dots::Event<>& event)
        {
            structList.update(event);

            if (!m_filterSettings.showEmpty == true &&
                ((event.isCreate() && structList.container().size() == 1) || 
                (event.isRemove() && structList.container().empty())))
            {
                m_typesChanged = true;
            }
        }));
    }
}

void TypeList::render()
{
    // init filter settings
    if (!m_filterSettingsInitialized)
    {
        m_filterSettingsInitialized = true;

        if (m_filterSettings.regexFilter.isValid())
        {
            const std::string& regexFilter = m_filterSettings.regexFilter;
            m_typeFilterBuffer.assign(std::max(regexFilter.size(), m_typeFilterBuffer.size()), '\0');
            std::copy(regexFilter.begin(), regexFilter.end(), m_typeFilterBuffer.begin());
        }
        else
        {
            m_filterSettings.regexFilter.construct();
        }

        m_filterSettings.showInternal.constructOrValue();
        m_filterSettings.showUncached.constructOrValue();
        m_filterSettings.showEmpty.constructOrValue();

        ImGui::SetKeyboardFocusHere();
    }

    // control area
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Filter");

        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F, false))
        {
            ImGui::SetKeyboardFocusHere();
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
        m_typesChanged |= ImGui::InputTextWithHint("##typeFilter", "<none>", m_typeFilterBuffer.data(), m_typeFilterBuffer.size());
        ImGui::PopItemWidth();
        
        {
            ImGui::SameLine();
            constexpr char ClearLabel[] = "Clear";

            if (m_typeFilterBuffer.front() == '\0')
            {
                ImGui::BeginDisabled();
                ImGui::Button(ClearLabel);
                ImGui::EndDisabled();
            }
            else
            {
                if (ImGui::Button(ClearLabel))
                {
                    m_typeFilterBuffer.assign(m_typeFilterBuffer.size(), '\0');
                    m_typesChanged = true;
                }
            }
        }

        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Types can be filtered by specifying substrings or ECMAScript regular expressions.");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Internal", &*m_filterSettings.showInternal))
        {
            m_typesChanged = true;
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Uncached", &*m_filterSettings.showUncached))
        {
            m_typesChanged = true;
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Empty", &*m_filterSettings.showEmpty))
        {
            m_typesChanged = true;
        }

        if (m_typesChanged)
        {
            m_structListsFiltered.clear();
            std::string_view typeFilter = m_typeFilterBuffer.data();
            m_filterSettings.regexFilter = typeFilter;
            std::regex regex{ typeFilter.data() };

            std::copy_if(m_structLists.begin(), m_structLists.end(), std::back_inserter(m_structListsFiltered), [&](const auto& structList)
            {
                const dots::type::StructDescriptor<>& descriptor = structList->container().descriptor();

                if (descriptor.internal() && !*m_filterSettings.showInternal)
                {
                    return false;
                }
                else if (!descriptor.cached() && !*m_filterSettings.showUncached)
                {
                    return false;
                }
                else if (descriptor.cached() && structList->container().empty() && !*m_filterSettings.showEmpty)
                {
                    return false;
                }
                else
                {
                    return typeFilter.empty() || std::regex_search(descriptor.name(), regex);
                }
            });
        }

        ImGui::SameLine();
        if (m_structListsFiltered.size() == 1)
        {
            ImGui::TextDisabled("(showing 1 type)");
        }
        else
        {
            ImGui::TextDisabled("(showing %zu types)", m_structListsFiltered.size());
        }
    }

    // struct lists
    constexpr ImGuiTableFlags TableFlags = 
        ImGuiTableFlags_Borders       |
        ImGuiTableFlags_BordersH      |
        ImGuiTableFlags_BordersOuterH |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_BordersV      |
        ImGuiTableFlags_BordersOuterV |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersOuter  |
        ImGuiTableFlags_BordersInner  |
        ImGuiTableFlags_ScrollY       |
        ImGuiTableFlags_Sortable      |
        ImGuiTableFlags_Hideable
    ;
    
    if (ImGui::BeginTable("Cached Types", 4, TableFlags, ImGui::GetContentRegionAvail()))
    {
        // create headers
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Activity", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Activity [dot]", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        // sort struct lists
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_typesChanged || sortSpecs->SpecsDirty)
        {
            std::sort(m_structListsFiltered.begin(), m_structListsFiltered.end(), [sortSpecs](const auto& lhs, const auto& rhs)
            {
                return lhs->less(*sortSpecs, *rhs);
            });

            m_typesChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render struct lists
        for (auto& structList : m_structListsFiltered) 
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            bool structListOpen = structList->renderBegin();

            if (ImGui::TableNextColumn())
            {
                structList->renderActivity();
            }

            if (ImGui::TableNextColumn())
            {
                structList->renderActivityDot();
            }

            if (ImGui::TableNextColumn())
            {
                ImGui::Text("%zu", structList->container().size());
            }

            if (structListOpen)
            {
                ImGui::TableNextColumn();
                structList->renderEnd();
            }
        }

        ImGui::EndTable();
    }
}
