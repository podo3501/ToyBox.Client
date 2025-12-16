#pragma once
#include "Shared/Utils/StlTypeExt.hpp"

class SerializerIO;
class AutoNamer;
class ComponentNameGenerator
{
public:
    ~ComponentNameGenerator();
    ComponentNameGenerator();
    ComponentNameGenerator(const ComponentNameGenerator& other);
    ComponentNameGenerator& operator=(const ComponentNameGenerator& other);

    bool operator==(const ComponentNameGenerator& other) const noexcept;
    string MakeNameFromComponent(const string& name) noexcept;
    string MakeNameFromBase(const string& name) noexcept;
    bool Remove(const string& name) noexcept;
    bool IsUnusedName(string_view name) const noexcept;
    void ProcessIO(SerializerIO& serializer);

private:
    unordered_svmap<string, unique_ptr<AutoNamer>> m_namers;
};