#include <widgets/dialogs/FileDialog.h>
#include <chrono>
#include <sstream>
#include <fmt/format.h>
#include <imgui.h>

FileDialog::FileDialog(std::string_view headerLabel, std::string confirmButtonLabel, bool fileMustExist) :
    m_sortCurrentFiles(false),
    m_highlightedSubdir(None),
    m_highlightedFile(None),
    m_selectedPathBuffer(1024, '\0'),
    m_confirmButtonWidth(60.f),
    m_headerLabel{ fmt::format("{}##FileDialog_Popup", headerLabel) },
    m_confirmButtonLabel{ std::move(confirmButtonLabel) },
    m_fileMustExist(fileMustExist)
{
    changeDirectory(std::filesystem::current_path());
    selectPath(m_currentDirectory);
    ImGui::OpenPopup(m_headerLabel.data());
}

const std::optional<std::filesystem::path>& FileDialog::file() const
{
    return m_confirmedFile;
}

bool FileDialog::render()
{
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
    ImGui::SetNextWindowPos(ImVec2{ viewportSize.x / 2, viewportSize.y / 2 }, 0, ImVec2{ 0.5f, 0.5f });
    ImGui::SetNextWindowSizeConstraints(ImVec2{ viewportSize.x * 0.45f, viewportSize.y * 0.45f }, ImVec2{ viewportSize.x * 0.9f, viewportSize.y * 0.9f });

    if (ImGui::BeginPopupModal(m_headerLabel.data()))
    {
        // directories table
        constexpr ImGuiTableFlags DirectoriesTableFlags = 
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_ScrollY
        ;

        if (ImGui::BeginTable("Directories", 1, DirectoriesTableFlags, ImVec2{ ImGui::GetContentRegionAvail().x * 0.3f, ImGui::GetContentRegionAvail().y - 50.0f }))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Directory", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    changeDirectory(m_currentDirectory.parent_path());
                    selectPath(m_currentDirectory);
                    ImGui::SetScrollHereY(0.0f);
                }
            }

            for (size_t i = 0; i < m_currentSubdirectories.size(); ++i)
            {
                const std::filesystem::path& path = m_currentSubdirectories[i].path();

                ImGui::TableNextColumn();
                if (ImGui::Selectable(path.stem().string().data(), i == m_highlightedSubdir, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        changeDirectory(path);
                        selectPath(m_currentDirectory);
                        ImGui::SetScrollHereY(0.0f);
                    }
                    else
                    {
                        selectPath(path);
                        m_highlightedSubdir = i;
                        m_highlightedFile = None;
                    }
                }
            }

            ImGui::EndTable();
        }

        ImGui::SameLine();

        // files table
        constexpr ImGuiTableFlags FilesTableFlags = 
            ImGuiTableFlags_BordersOuter      |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY           |
            ImGuiTableFlags_Resizable         |
            ImGuiTableFlags_Sortable          |
            ImGuiTableFlags_SortMulti         |
            ImGuiTableFlags_Reorderable       |
            ImGuiTableFlags_Hideable
        ;

        if (ImGui::BeginTable("Files", 4, FilesTableFlags, ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 50.0f }))
        {
            enum Column{ NameColumn, SizeColumn, TypeColumn, DateColumn };
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Size");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Date modified");
            ImGui::TableHeadersRow();

            // sort files
            if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_sortCurrentFiles || sortSpecs->SpecsDirty)
            {
                std::sort(m_currentFiles.begin(), m_currentFiles.end(), [sortSpecs](auto& lhs, auto& rhs)
                {
                    for (int i = 0; i < sortSpecs->SpecsCount; ++i)
                    {
                        const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs->Specs[i];

                        auto compare = [&sortSpec](const auto& lhs, const auto& rhs)
                        {
                            if (sortSpec.SortDirection == ImGuiSortDirection_Ascending)
                                return std::less{}(lhs, rhs);
                            else
                                return std::greater{}(lhs, rhs);
                        };

                        switch (static_cast<Column>(sortSpec.ColumnIndex))
                        {
                            case NameColumn:
                            {
                                std::string lhsName = lhs.path().filename().string();
                                std::string rhsName = rhs.path().filename().string();

                                if (compare(lhsName, rhsName))
                                    return true;
                                else if (compare(rhsName, lhsName))
                                    return false;
                                break;
                            }

                            case SizeColumn:
                                if (compare(lhs.file_size(), rhs.file_size()))
                                    return true;
                                else if (compare(rhs.file_size(), lhs.file_size()))
                                    return false;
                                break;

                            case TypeColumn:
                                if (compare(lhs.path().extension(), rhs.path().extension()))
                                    return true;
                                else if (compare(rhs.path().extension(), lhs.path().extension()))
                                    return false;
                                break;

                            case DateColumn:
                                if (compare(lhs.last_write_time(), rhs.last_write_time()))
                                    return true;
                                else if (compare(rhs.last_write_time(), lhs.last_write_time()))
                                    return false;
                                break;

                            default: 
                                break;
                        }
                    }

                    return lhs.path().filename().string() < rhs.path().filename().string();
                });

                sortSpecs->SpecsDirty = false;
            }

            for (size_t i = 0; i < m_currentFiles.size(); ++i)
            {
                const std::filesystem::directory_entry& entry = m_currentFiles[i];
                const std::filesystem::path& path = entry.path();

                ImGui::TableNextColumn();
                if (ImGui::Selectable(path.filename().string().data(), i == m_highlightedFile, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    selectPath(path);
                    m_highlightedSubdir = None;
                    m_highlightedFile = i;

                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        m_confirmedFile.emplace(m_selectedPath);
                }

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(std::to_string(entry.file_size()).data());

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(path.extension().string().data());

                ImGui::TableNextColumn();
                std::string lastWriteTimestamp = [&entry]
                {
                    using file_time_t = std::filesystem::file_time_type;
                    file_time_t lastWriteFileTime = entry.last_write_time();
                    auto lastWriteTimePoint = std::chrono::time_point_cast<std::chrono::system_clock::duration>(lastWriteFileTime - file_time_t::clock::now() + std::chrono::system_clock::now());

                    std::time_t lastWriteTime = std::chrono::system_clock::to_time_t(lastWriteTimePoint);
                    std::tm* lastWriteTm = std::localtime(&lastWriteTime);
                    std::ostringstream oss;
                    oss << std::put_time(lastWriteTm, "%F %R");

                    return oss.str();
                }();
                ImGui::TextUnformatted(lastWriteTimestamp.data());
            }

            ImGui::EndTable();
        }

        // selected path input
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("File name:");

            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            if (ImGui::InputText("##SelectedPathInput", m_selectedPathBuffer.data(), m_selectedPathBuffer.size()))
                selectPathFromBuffer();
        }

        // confirm/cancel buttons
        {
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 70.0f - m_confirmButtonWidth);

            ImGui::BeginDisabled(!is_regular_file(m_selectedPath) && (m_fileMustExist || exists(m_selectedPath)));
            if (ImGui::Button(m_confirmButtonLabel.data()))
                m_confirmedFile.emplace(m_selectedPath);
            m_confirmButtonWidth = ImGui::GetItemRectSize().x;
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
                ImGui::CloseCurrentPopup();
        }

        if (m_confirmedFile)
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
        return true;
    }
    else
        return false;
}

void FileDialog::changeDirectory(std::filesystem::path path)
{
    m_highlightedSubdir = None;
    m_highlightedFile = None;
    m_currentSubdirectories.clear();
    m_currentFiles.clear();
    m_currentDirectory = std::move(path);

    try
    {
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator{ m_currentDirectory, std::filesystem::directory_options::skip_permission_denied })
        {
            if (entry.is_directory())
                m_currentSubdirectories.emplace_back(std::move(entry));
            else if (entry.is_regular_file())
            {
                std::error_code fileSizeError;
                std::error_code lastWriteTimeError;
                (void)entry.file_size(fileSizeError);
                (void)entry.last_write_time(lastWriteTimeError);

                // note that this is necessary to skip files with incompatible
                // access permissions
                if (!fileSizeError && !lastWriteTimeError)
                    m_currentFiles.emplace_back(std::move(entry));
            }
        }
    }
    catch (...)
    {
        /* do nothing */
    }

    std::sort(m_currentSubdirectories.begin(), m_currentSubdirectories.end(), [](auto& lhs, auto& rhs)
    {
        return lhs.path().filename() < rhs.path().filename();
    });

    m_sortCurrentFiles = true;
}

void FileDialog::selectPath(std::filesystem::path path)
{
    m_selectedPath = std::move(path);

    std::string currentPath = m_selectedPath.string();
    if (is_directory(m_selectedPath) && m_selectedPath != m_selectedPath.root_path())
        currentPath += std::filesystem::path::preferred_separator;
    currentPath += '\0';

    std::copy(currentPath.begin(), currentPath.end(), m_selectedPathBuffer.begin());
}

void FileDialog::selectPathFromBuffer()
{
    m_highlightedSubdir = None;
    m_highlightedFile = None;
    m_selectedPath = m_selectedPathBuffer.data();

    if (std::filesystem::path parent = m_selectedPath.parent_path(); parent != m_currentDirectory && exists(parent) && is_directory(parent))
        changeDirectory(parent);

    if (exists(m_selectedPath))
    {
        auto set_highlight = [this](const auto& entries, size_t& highlightIndex)
        {
            for (size_t i = 0; const std::filesystem::directory_entry& entry : entries)
            {
                if (entry.path() == m_selectedPath)
                {
                    highlightIndex = i;
                    break;
                }

                ++i;
            }
        };

        if (is_directory(m_selectedPath))
            set_highlight(m_currentSubdirectories, m_highlightedSubdir);
        else if (is_regular_file(m_selectedPath))
            set_highlight(m_currentFiles, m_highlightedFile);
    }
}
