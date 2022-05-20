#pragma once
#include <dots/type/AnyStruct.h>

namespace dots::type
{
    Struct& assign_all(Struct& instance, const Struct& other);
    AnyStruct copy_all(const Struct& other);
}
