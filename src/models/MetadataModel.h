#pragma once
#include <array>
#include <common/ImGuiExt.h>
#include <common/Colors.h>
#include <dots/dots.h>
#include <models/PublisherModel.h>

struct MetadataModel
{
    MetadataModel(const PublisherModel& publisherModel, const dots::Event<>& event);

    DotsMt lastOperation() const;
    dots::timepoint_t lastPublished() const;
    dots::uint32_t lastPublishedBy() const;
    dots::property_set_t lastPublishedProperties() const;

    const ImGuiExt::ColoredText& lastOperationText() const;
    const ImGuiExt::ColoredText& lastPublishedText() const;
    const ImGuiExt::ColoredText& lastPublishedByText() const;

private:

    enum MetaData { LastPublished, LastPublishedBy, MetaDataSize };

    inline static std::array<ImGuiExt::ColoredText, 3> LastOperationTexts{
        ImGuiExt::ColoredText{ "CREATE", ColorThemeActive.Create },
        ImGuiExt::ColoredText{ "UPDATE", ColorThemeActive.Update },
        ImGuiExt::ColoredText{ "REMOVE", ColorThemeActive.Remove }
    };

    struct data
    {
        DotsMt lastOperation;
        dots::timepoint_t lastPublished;
        dots::uint32_t lastPublishedBy;
        dots::property_set_t lastPublishedProperties;
        ImGuiExt::ColoredText lastPublishedText;
        PublisherModel publisherModel;
    };

    std::shared_ptr<const data> m_data;
};
