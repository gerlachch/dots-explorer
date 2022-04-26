#pragma once
#include <vector>
#include <common/ImGuiExt.h>
#include <dots/dots.h>

struct MetadataModel
{
    enum MetaData { LastOp, LastPublished, LastPublishedBy, MetaDataSize };

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

    DotsMt m_lastOperation;
    dots::timepoint_t m_lastPublished;
    dots::uint32_t m_lastPublishedBy;
    mutable std::vector<ImGuiExt::ColoredText> m_metadataText;
};
