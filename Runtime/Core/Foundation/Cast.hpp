#pragma once
#include "../Foundation/Types.h"
#include <type_traits>
#include <concepts>

// 사용법은 std::static_pointer_cast와 동일하지만, 실제 타입을 확인한 후 변환하므로 타입이 다르면 nullptr을 반환한다.
// Cast<Derived>(Parent); // 단일 Raw Pointer
// Cast<Derived>(Parent); // 단일 shared_ptr
// CastAll<Derived>(Parents); // 여러 shared_ptr

namespace Core
{
    template<typename T>
    concept TypeCastable =
        requires(const T & obj)
    {
        { obj.GetTypeID() } -> std::convertible_to<TypeID>;
        { obj.IsKindOf(TypeID{}) } -> std::convertible_to<bool>;
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

        if (!source->IsKindOf(TargetType::StaticTypeID()))
            return nullptr;

        return static_cast<TargetType*>(source);
    }

    template<CastTarget TargetType, TypeCastable SourceType>
    inline const TargetType* Cast(const SourceType* source)
    {
        if (!source)
            return nullptr;

        if (!source->IsKindOf(TargetType::StaticTypeID()))
            return nullptr;

        return static_cast<const TargetType*>(source);
    }

    template<CastTarget TargetType, TypeCastable SourceType>
    inline std::shared_ptr<TargetType> Cast(const std::shared_ptr<SourceType>& source)
    {
        if (!source)
            return nullptr;

        if (!source->IsKindOf(TargetType::StaticTypeID()))
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