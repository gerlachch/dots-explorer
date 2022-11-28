#pragma once
#include <string_view>
#include <vector>
#include <optional>
#include <filesystem>
#include <WidgetSettings.dots.h>

struct FileDialog
{
    FileDialog(std::string_view headerLabel, std::string confirmButtonLabel, bool fileMustExist);

    const std::optional<std::filesystem::path>& file() const;
    bool render();

private:

    static constexpr size_t None = std::numeric_limits<size_t>::max();

    void changeDirectory(std::filesystem::path path);
    void selectPath(std::filesystem::path path);
    void selectPathFromBuffer();

    std::optional<std::filesystem::path> m_confirmedFile;
    std::filesystem::path m_currentDirectory;
    std::vector<std::filesystem::directory_entry> m_currentSubdirectories;
    std::vector<std::filesystem::directory_entry> m_currentFiles;
    std::reference_wrapper<WidgetSettings> m_widgetSettings;
    bool m_sortCurrentFiles;
    size_t m_highlightedSubdir;
    size_t m_highlightedFile;
    std::filesystem::path m_selectedPath;
    std::string m_selectedPathBuffer;
    float m_confirmButtonWidth;
    std::string m_headerLabel;
    std::string m_confirmButtonLabel;
    bool m_fileMustExist;
};

struct FileOpenDialog : FileDialog
{
    FileOpenDialog(std::string headerLabel = "Open File") :
        FileDialog(std::move(headerLabel), "Open", true)
    {
        /* do nothing */
    }
};

struct FileSaveDialog : FileDialog
{
    FileSaveDialog(std::string headerLabel = "Save File") :
        FileDialog(std::move(headerLabel), "Save", false)
    {
        /* do nothing */
    }
};
