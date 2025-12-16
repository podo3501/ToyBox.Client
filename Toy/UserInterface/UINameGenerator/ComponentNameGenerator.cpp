#include "pch.h"
#include "AutoNamer.h"
#include "ComponentNameGenerator.h"
#include "Shared/Utils/StringExt.h"
#include "Shared/Utils/StlExt.h"
#include "Shared/SerializerIO/SerializerIO.h"

ComponentNameGenerator::~ComponentNameGenerator() = default;
ComponentNameGenerator::ComponentNameGenerator() = default;

ComponentNameGenerator::ComponentNameGenerator(const ComponentNameGenerator& other)
{
    m_namers = CopyAssoc(other.m_namers);
}

bool ComponentNameGenerator::operator==(const ComponentNameGenerator& other) const noexcept
{
    return CompareUnorderedAssoc(m_namers, other.m_namers);
}

ComponentNameGenerator& ComponentNameGenerator::operator=(const ComponentNameGenerator& other)
{
    if (this == &other) return *this;
    
    m_namers = CopyAssoc(other.m_namers);

    return *this;
}

string ComponentNameGenerator::MakeNameFromComponent(const string& name) noexcept
{
    auto& autoNamer = TryEmplaceAssoc(m_namers, name);
    return AppendIfPresent(name, autoNamer->Generate());
}

string ComponentNameGenerator::MakeNameFromBase(const string& name) noexcept
{
    auto [baseName, _] = ExtractNameAndId(name);
    auto& autoNamer = TryEmplaceAssoc(m_namers, baseName);

    return AppendIfPresent(baseName, autoNamer->Generate());
}

template<typename T>
bool IsVaildEnumType(T type)
{
    constexpr auto size = EnumSize<T>();
    if (size <= static_cast<size_t>(type))
        return false;

    return true;
}

bool ComponentNameGenerator::Remove(const string& name) noexcept
{
    if (name.empty()) return true;

    auto [baseName, id] = ExtractNameAndId(name);
    if (baseName.empty()) return false;

    auto [result, deletable] = m_namers[baseName]->Recycle(id);
    ReturnIfFalse(result);

    if (deletable) m_namers.erase(baseName);

    return true;
}

bool ComponentNameGenerator::IsUnusedName(string_view name) const noexcept
{
    auto [baseName, id] = ExtractNameAndId(name);
    if (baseName.empty()) return true;

    auto find = m_namers.find(baseName);
    if (find == m_namers.end()) return true;

    return !find->second->IsUsed(id);
}

void ComponentNameGenerator::ProcessIO(SerializerIO& serializer)
{
    serializer.Process("Namers", m_namers);
}