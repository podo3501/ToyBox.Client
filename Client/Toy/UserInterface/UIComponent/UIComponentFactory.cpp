#include "pch.h"
#include "UIComponentFactory.h"
#include "Components/Panel.h"
#include "Components/PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "Components/PatchTexture/PatchTextureStd/PatchTextureStd3.h"
#include "Components/PatchTexture/PatchTextureStd/PatchTextureStd9.h"
#include "Components/PatchTexture/PatchTextureLite/PatchTextureLite1.h"
#include "Components/PatchTexture/PatchTextureLite/PatchTextureLite3.h"
#include "Components/PatchTexture/PatchTextureLite/PatchTextureLite9.h"
#include "Components/Button.h"
#include "Components/TextArea.h"
#include "Components/Dialog.h"
#include "Components/ListArea.h"
#include "Components/Container.h"
#include "Components/RenderTexture.h"
#include "Components/ScrollBar.h"
#include "Components/TextureSwitcher.h"
#include "Components/UIModuleAsComponent.h"
#include <iterator>

using FactoryFunc = unique_ptr<UIComponent>(*)();   //constexpr을 쓰기 때문에 function(동적할당)을 쓸 수없다.
constexpr FactoryFunc ComponentFactory[] = //enum의 값과 일치가 되어야 한다. 아니면 if로 해야 한다.
{
    []() -> unique_ptr<UIComponent> { return make_unique<Panel>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<PatchTextureStd1>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<PatchTextureStd3>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<PatchTextureStd9>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<PatchTextureLite1>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<PatchTextureLite3>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<PatchTextureLite9>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<Button>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<TextArea>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<Dialog>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<ListArea>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<Container>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<RenderTexture>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<ScrollBar>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<TextureSwitcher>(); },
    []() -> unique_ptr<UIComponent> { return make_unique<UIModuleAsComponent>(); },
    //[]() -> unique_ptr<UIComponent> { return make_unique<Unknown>(); } //이런건 없다.
};

static_assert((size(ComponentFactory) + 1) == EnumSize<ComponentID>(), "ComponentFactory 크기와 ComponentID enum 크기가 일치하지 않습니다!");

unique_ptr<UIComponent> CreateComponent(const string& typeName)
{
    auto type = StringToEnum<ComponentID>(typeName);
    if (!type) return nullptr;

    int componentID = EtoV(*type);
    return ComponentFactory[componentID]();
}