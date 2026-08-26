#pragma once

enum class ViewID : uint32_t
{
    Main,
    Secondary,
    EditorMain,
    EditorSecondary,
    Extra0,
    Extra1,
    Extra2,
    Extra3,
    Extra4,
    Extra5,
    Count = 10,
};

constexpr std::string_view ToString(ViewID id) noexcept
{
    switch (id)
    {
    case ViewID::Main: return "Main";
    case ViewID::Secondary: return "Secondary";
    case ViewID::EditorMain: return "EditorMain";
    case ViewID::EditorSecondary: return "EditorSecondary";
    case ViewID::Extra0: return "Extra0";
    case ViewID::Extra1: return "Extra1";
    case ViewID::Extra2: return "Extra2";
    case ViewID::Extra3: return "Extra3";
    case ViewID::Extra4: return "Extra4";
    case ViewID::Extra5: return "Extra5";
    }
    return "Unknown";
}