#include <dots_ext/struct_ops.h>

namespace dots::type
{
    Struct& assign_all(Struct& instance, const Struct& other)
    {
        PropertyArea& thisPropertyArea = instance._propertyArea();
        const PropertyArea& otherPropertyArea = other._propertyArea();

        PropertySet& thisValidProperties = thisPropertyArea.validProperties();

        for (const PropertyDescriptor& propertyDescriptor : instance._propertyDescriptors())
        {
            Typeless& valueThis = thisPropertyArea.getProperty<Typeless>(propertyDescriptor.offset());
            const Typeless& valueOther = otherPropertyArea.getProperty<Typeless>(propertyDescriptor.offset());

            if (propertyDescriptor.set() <= other._validProperties())
            {
                if (propertyDescriptor.valueDescriptor().type() == Type::Struct)
                {
                    propertyDescriptor.valueDescriptor().constructInPlace(valueThis);
                    assign_all(valueThis.to<Struct>(), valueOther.to<Struct>());
                }
                else
                {
                    propertyDescriptor.valueDescriptor().constructInPlace(valueThis, valueOther);
                }

                thisValidProperties += propertyDescriptor.set();
            }
        }

        return instance;
    }

    AnyStruct copy_all(const Struct& other)
    {
        AnyStruct instance{ other._descriptor() };
        assign_all(instance, other);

        return instance;
    }

    Struct& default_init(Struct& instance)
    {
        for (auto& property : instance)
        {
            property.constructOrValue();

            if (property.descriptor().valueDescriptor().type() == Type::Struct)
            {
                default_init(property->to<Struct>());
            }
        }

        return instance;
    }
}
