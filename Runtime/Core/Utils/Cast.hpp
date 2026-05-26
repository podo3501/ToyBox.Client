#pragma once
#include "../Foundation/Types.h"
#include <type_traits>
#include <concepts>

namespace Core
{
    template<typename T>
    concept TypeCastable =
        requires(const T & obj)
    {
        { obj.GetTypeID() } -> std::convertible_to<TypeID>;
    };

    template<typename T>
    concept HasStaticTypeID =
        requires
    {
        { T::StaticTypeID() } -> std::convertible_to<TypeID>;
    };

    template<typename T>
    concept CastTarget =
        std::is_pointer_v<T> &&
        HasStaticTypeID<std::remove_pointer_t<T>>;

    template<CastTarget TargetType, TypeCastable SourceType>
    inline TargetType Cast(SourceType* source)
    {
        if (!source)
            return nullptr;

        using Target = std::remove_pointer_t<TargetType>;

        if (source->GetTypeID() != Target::StaticTypeID())
            return nullptr;

        return static_cast<TargetType>(source);
    }

    template<CastTarget TargetType, TypeCastable SourceType>
    inline const std::remove_pointer_t<TargetType>* Cast(const SourceType* source)
    {
        if (!source)
            return nullptr;

        using Target = std::remove_pointer_t<TargetType>;

        if (source->GetTypeID() != Target::StaticTypeID())
            return nullptr;

        return static_cast<const Target*>(source);
    }
}