#pragma once
#include "Shared/Utils/StlTypeExt.hpp"

class SerializerIO;
class AutoNamer;
class ComponentNameGenerator;
enum class ComponentID;
class UINameGenerator
{
protected:
    UINameGenerator(const UINameGenerator& other);

public:
	~UINameGenerator();
	UINameGenerator();
    bool operator==(const UINameGenerator& other) const noexcept;
    unique_ptr<UINameGenerator> Clone() const;

    optional<string> MakeRegionOf(const string& region) noexcept;
    bool RemoveRegion(const string& region) noexcept;
    bool IsUnusedRegion(string_view region) noexcept;

    optional<pair<string, string>> MakeNameOf(const string& region, const string& name, 
        bool forceUniqueRegion = false) noexcept;
    bool RemoveName(const string& region, const string& name) noexcept;
    bool IsUnusedName(string_view region, string_view name) noexcept;

    void ProcessIO(SerializerIO& serializer);

private:
    ComponentNameGenerator* GetComponentNameGen(string_view region) const noexcept;

    unordered_svmap<string, unique_ptr<AutoNamer>> m_regionGens; //Generator 해서 키 값을 생성함. 이 키값과 region키 값이 다름. 여기는 base name 으로 키값을 생성하는 곳.
    unordered_svmap<string, unique_ptr<ComponentNameGenerator>> m_componentNameGens;
};
