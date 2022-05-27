#pragma once
#include <dots/type/AnyStruct.h>

namespace dots::type
{
    Struct& assign_all(Struct& instance, const Struct& other);
    AnyStruct copy_all(const Struct& other);

    Struct& default_init(Struct& instance);

    template <typename T> T& default_init()
    {
        T instance;
        default_init(instance).template _to<T>();

        return instance;
    }
}
