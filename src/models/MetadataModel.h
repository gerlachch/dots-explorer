#pragma once
#include <array>
#include <common/ImGuiExt.h>
#include <common/Colors.h>
#include <dots/dots.h>
#include <models/PublisherModel.h>

struct MetadataModel
{
    MetadataModel(const PublisherModel& publisherModel);

    DotsMt lastOperation() const;
    dots::timepoint_t lastPublished() const;
    dots::uint32_t lastPublishedBy() const;
    dots::property_set_t lastPublishedProperties() const;

    const ImGuiExt::ColoredText& lastOperationText() const;
    const ImGuiExt::ColoredText& lastPublishedText() const;
    const ImGuiExt::ColoredText& lastPublishedByText() const;

    void fetch(const dots::Event<>& event);

private:

    enum MetaData { LastPublished, LastPublishedBy, MetaDataSize };

    using publisher_model_ref_t = std::reference_wrapper<const PublisherModel>;

    inline static std::array<ImGuiExt::ColoredText, 3> LastOperationTexts{
        ImGuiExt::ColoredText{ "CREATE", ColorThemeActive.Create },
        ImGuiExt::ColoredText{ "UPDATE", ColorThemeActive.Update },
        ImGuiExt::ColoredText{ "REMOVE", ColorThemeActive.Remove }
    };

    DotsMt m_lastOperation;
    dots::timepoint_t m_lastPublished;
    dots::uint32_t m_lastPublishedBy;
    dots::property_set_t m_lastPublishedProperties;
    mutable ImGuiExt::ColoredText m_lastPublishedText;
    publisher_model_ref_t m_publisherModel;
};
