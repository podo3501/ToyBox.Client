#include "pch.h"
#include "UINameGenerator.h"
#include "AutoNamer.h"
#include "ComponentNameGenerator.h"
#include "../UIComponent/UIType.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "Shared/Utils/StringExt.h"
#include "Shared/Utils/StlExt.h"

UINameGenerator::~UINameGenerator() = default;
UINameGenerator::UINameGenerator() = default;

UINameGenerator::UINameGenerator(const UINameGenerator& other)
{
    m_regionGens = CopyAssoc(other.m_regionGens);
    m_componentNameGens = CopyAssoc(other.m_componentNameGens);
}

bool UINameGenerator::operator==(const UINameGenerator& other) const noexcept
{
    ReturnIfFalse(CompareUnorderedAssoc(m_regionGens, other.m_regionGens));
    ReturnIfFalse(CompareUnorderedAssoc(m_componentNameGens, other.m_componentNameGens));

    return true;
}

unique_ptr<UINameGenerator> UINameGenerator::Clone() const
{
    return unique_ptr<UINameGenerator>(new UINameGenerator(*this));
}

optional<string> UINameGenerator::MakeRegionOf(const string& region) noexcept
{
    if (region.empty() && !IsUnusedRegion(region))
        return nullopt; //region이 ""인 경우는 변형된 region name을 만들지 않는다.

    auto [name, id] = ExtractNameAndId(region);
    auto& nameGenerator = TryEmplaceAssoc(m_regionGens, name);
    string newRegion = AppendIfPresent(name, nameGenerator->Generate());
    if (!IsUnusedRegion(newRegion)) return "";

    m_componentNameGens.emplace(newRegion, make_unique<ComponentNameGenerator>());
    return newRegion;
}

bool UINameGenerator::RemoveRegion(const string& region) noexcept
{
    if (IsUnusedRegion(region)) return true;

    auto [name, id] = ExtractNameAndId(region);
    auto find = m_regionGens.find(name);
    if (find == m_regionGens.end()) return false; //키값은 있는데 Generator가 없으면 이상한 일이다.

    auto [result, deletable] = find->second->Recycle(id);
    ReturnIfFalse(result);

    m_componentNameGens.erase(region);
    if (deletable) m_regionGens.erase(name);
    return true;
}

bool UINameGenerator::IsUnusedRegion(string_view region) noexcept
{
    return m_componentNameGens.find(region) == m_componentNameGens.end();
}

static string GetComponentName(const string& name)
{
    if (name.empty()) return "";

    auto [curName, id] = ExtractNameAndId(name);
    if (!IsValidEnumString<ComponentID>(curName))
        return "";
    
    return curName;
}

optional<pair<string, string>> UINameGenerator::MakeNameOf(const string& region, const string& name, bool forceUniqueRegion) noexcept
{
    string newRegion{ region }, newName{ name };
    if (IsUnusedRegion(region) || forceUniqueRegion) //붙일 region이 존재하지 않거나, 존재하더라도 유니크이어야 한다면
    {
        auto regionOpt = MakeRegionOf(region);
        if (!regionOpt) return nullopt;
        newRegion = regionOpt.value();
    }

    auto nameGen = GetComponentNameGen(newRegion);
    if (!nameGen) return nullopt;

    string strComponent = GetComponentName(name);
    if (!strComponent.empty())
        newName = nameGen->MakeNameFromComponent(strComponent);
    else
        newName = nameGen->MakeNameFromBase(name);

    return make_optional(make_pair(newRegion, newName));
}

bool UINameGenerator::RemoveName(const string& region, const string& name) noexcept
{
    auto nameGen = GetComponentNameGen(region);
    return nameGen ? nameGen->Remove(name) : true;
}

bool UINameGenerator::IsUnusedName(string_view region, string_view name) noexcept
{
    auto nameGen = GetComponentNameGen(region);
    return nameGen ? nameGen->IsUnusedName(name) : true;
}

ComponentNameGenerator* UINameGenerator::GetComponentNameGen(string_view region) const noexcept
{
    auto find = m_componentNameGens.find(region);
    if (find == m_componentNameGens.end()) return nullptr;

    return find->second.get();
}

void UINameGenerator::ProcessIO(SerializerIO& serializer)
{
    serializer.Process("RegionGens", m_regionGens);
    serializer.Process("RegionNames", m_componentNameGens);
}