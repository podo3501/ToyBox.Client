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
    concept CastTarget = HasStaticTypeID<T>;

    template<CastTarget TargetType, TypeCastable SourceType>
    inline TargetType* Cast(SourceType* source)
    {
        if (!source)
            return nullptr;

        if (source->GetTypeID() != TargetType::StaticTypeID())
            return nullptr;

        return static_cast<TargetType*>(source);
    }

    template<CastTarget TargetType, TypeCastable SourceType>
    inline const TargetType* Cast(const SourceType* source)
    {
        if (!source)
            return nullptr;

        if (source->GetTypeID() != TargetType::StaticTypeID())
            return nullptr;

        return static_cast<const TargetType*>(source);
    }

    template<CastTarget TargetType, TypeCastable SourceType>
    inline std::shared_ptr<TargetType> Cast(const std::shared_ptr<SourceType>& source)
    {
        if (!source)
            return nullptr;

        if (source->GetTypeID() != TargetType::StaticTypeID())
            return nullptr;

        return std::static_pointer_cast<TargetType>(source);
    }

    template<CastTarget TargetType, TypeCastable SourceType>
    inline std::vector<std::shared_ptr<TargetType>> CastAll(const std::vector<std::shared_ptr<SourceType>>& sources)
    {
        std::vector<std::shared_ptr<TargetType>> results;
        results.reserve(sources.size());

        for (const auto& source : sources)
            results.push_back(Core::Cast<TargetType>(source));

        return results;
    }
}