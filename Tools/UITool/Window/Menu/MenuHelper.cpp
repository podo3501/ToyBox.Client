#include "pch.h"
#include "MenuHelper.h"
#include "Config/Config.h"

using namespace Tool;

template<>
inline constexpr auto EnumUtil::EnumToStringMap<ResolutionType> = std::array{
    "2560x1440",
    "1920x1080",
    "1600x900",
    "1280x720",
    "1024x768",
    "800x600"
};
ASSERT_ENUM_COUNT(ResolutionType);

bool ResolutionSettingShow()
{
    if (!ImGui::BeginMenu("Resolution"))
        return false;

    auto select{ false };
    auto selectedResolution = Config::GetResolution();
    for(auto curType : EnumUtil::EnumValues<ResolutionType>())
    {
        bool isSelected = (selectedResolution == curType);
        if (ImGui::MenuItem(EnumUtil::EnumToString(curType).c_str(), nullptr, isSelected))
        {
            Config::SetResolution(curType);
            select = true;
        }
    }

    ImGui::EndMenu();

    return select;
}
