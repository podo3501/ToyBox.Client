#include "pch.h"
#include "MenuHelper.h"
#include "Config/Config.h"
#include "Shared/Utils/EnumHelpers.h"

using namespace Tool;

template<>
constexpr std::size_t EnumSize<ResolutionType>() { return 6; }

template<>
constexpr auto EnumToStringMap<ResolutionType>()->std::array<const char*, EnumSize<ResolutionType>()> {
    return { {
        { "2560x1440" },
        { "1920x1080" },
        { "1600x900" },
        { "1280x720" },
        { "1024x768" },
        { "800x600" },
    } };
}

bool ResolutionSettingShow()
{
    if (!ImGui::BeginMenu("Resolution"))
        return false;

    auto select{ false };
    auto selectedResolution = Config::GetResolution();
    for (auto i : views::iota(0u, EnumSize<ResolutionType>()))
    {
        ResolutionType curType = static_cast<ResolutionType>(i);
        bool isSelected = (selectedResolution == curType);
        
        if (ImGui::MenuItem(EnumToString(curType).c_str(), nullptr, isSelected))
        {
            Config::SetResolution(curType);
            select = true;
        }
    }

    ImGui::EndMenu();

    return select;
}
