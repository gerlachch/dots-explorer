#pragma once
#include <vector>
#include <array>
#include <common/ImGuiExt.h>
#include <common/Colors.h>
#include <dots/dots.h>

struct MetadataModel
{
    MetadataModel();

    DotsMt lastOperation() const;
    dots::timepoint_t lastPublished() const;
    dots::uint32_t lastPublishedBy() const;

    const std::vector<ImGuiExt::ColoredText>& metadataText() const;
    const ImGuiExt::ColoredText& lastOperationText() const;
    const ImGuiExt::ColoredText& lastPublishedText() const;
    const ImGuiExt::ColoredText& lastPublishedByText() const;

    void fetch(const dots::Event<>& event);

private:

    enum MetaData { LastPublished, LastPublishedBy, MetaDataSize };

    inline static std::array<ImGuiExt::ColoredText, 3> LastOperationTexts{
        ImGuiExt::ColoredText{ "CREATE", ColorThemeActive.Create },
        ImGuiExt::ColoredText{ "UPDATE", ColorThemeActive.Update },
        ImGuiExt::ColoredText{ "REMOVE", ColorThemeActive.Remove }
    };

    DotsMt m_lastOperation;
    dots::timepoint_t m_lastPublished;
    dots::uint32_t m_lastPublishedBy;
    mutable std::vector<ImGuiExt::ColoredText> m_metadataText;
};
