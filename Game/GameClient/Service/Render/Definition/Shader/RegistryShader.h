#pragma once
#include "ShaderTypes.h"

namespace RegistryShader //이 enum 값은 파일 저장이나 직렬화, 네트워크의 값으로 사용되면 안된다. inspectorImage가 release 일때 빠질수도 있다.
{
    constexpr ShaderID Shadow{ 1 };
    constexpr ShaderID Phong{ 2 };
    constexpr ShaderID PBR{ 3 };
    constexpr ShaderID Grid{ 4 };
    constexpr ShaderID UI{ 5 };
    constexpr ShaderID Skybox{ 6 };
    constexpr ShaderID Composite{ 7 }; // view들을 합성.
    constexpr ShaderID MipGenerator{ 8 }; //compute

    constexpr ShaderID InspectorImage{ 9 }; //테스트용 shader. 이건 release 일때 빠질수도 있다.
}