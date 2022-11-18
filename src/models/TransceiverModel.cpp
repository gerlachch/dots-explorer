#include <models/TransceiverModel.h>
#include <StructDescriptorData.dots.h>
#include <EnumDescriptorData.dots.h>

TransceiverModel::TransceiverModel() :
    m_eventIndex(0)
{
    m_subscriptions.emplace_back(dots::subscribe<StructDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<EnumDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<dots::type::StructDescriptor>({ &TransceiverModel::update, this }));
}

void TransceiverModel::subscribe(const StructDescriptorModel& descriptorModel, event_handler_t handler)
{
    m_eventHandlersTyped[&descriptorModel].emplace_back(std::move(handler));
}

void TransceiverModel::subscribe(event_handler_t handler)
{
    m_eventHandlers.emplace_back(std::move(handler));
}

void TransceiverModel::subscribe(struct_type_handler_t handler)
{
    m_structTypeHandlers.emplace_back(std::move(handler));
}

void TransceiverModel::update(const dots::type::StructDescriptor& descriptor)
{
    if (!descriptor.substructOnly())
    {
        const StructDescriptorModel& descriptorModel = m_descriptorModels.emplace_back(descriptor);

        for (auto& handler : m_structTypeHandlers)
        {
            handler(descriptorModel);
        }

        m_subscriptions.emplace_back(dots::subscribe(descriptor, [this, &descriptorModel](const dots::Event<>& event)
        {
            auto eventModel = std::make_shared<EventModel>(++m_eventIndex, m_publisherModel, descriptorModel, event);

            for (auto& handler : m_eventHandlers)
            {
                handler(eventModel);
            }

            if (auto it = m_eventHandlersTyped.find(&descriptorModel); it != m_eventHandlersTyped.end())
            {
                for (auto& handler : it->second)
                {
                    handler(eventModel);
                }
            }
        }));
    }
}
