#include "pch.h"
#include "ComponentTestScene.h"
#include "IRenderer.h"
#include "Shared/Utils/GeometryExt.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include "UserInterface/UIModule.h"
#include "UserInterface/UIComponentLocator.h"
#include "UserInterface/UIComponent/Traverser/UITraverser.h"
#include "UserInterface/UIComponent/UIUtility.h"
#include "UserInterface/UIComponent/Components/Panel.h"
#include "UserInterface/UIComponent/Components/ListArea.h"
#include "UserInterface/UIComponent/Components/Button.h"
#include "UserInterface/UIComponent/Components/TextArea.h"
#include "UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd9.h"
#include "UserInterface/UIComponent/Components/SampleComponent.h"
#include "UserInterface/UIComponent/Components/TextureSwitcher.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"

using namespace UITraverser;

ComponentTestScene::ComponentTestScene(IRenderer* renderer) :
	Scene(renderer)
{} 

bool ComponentTestScene::Enter()
{
    IRenderer* renderer = GetRenderer();
    UILayout layout{ GetSizeFromRectangle(GetRectResolution()) };

    auto texResBinder = CreateTextureResourceBinder(L"UI/SampleTexture/SampleTextureBinder.json", renderer);
    m_uiModule = CreateUIModule("ComponentTest", layout, "Main", move(texResBinder));
    return LoadResources();
}

bool ComponentTestScene::Leave()
{
    return ReleaseUIModule("ComponentTest");
}

bool ComponentTestScene::LoadResources()
{
    AttachComponentToPanel(CreateComponent<TextureSwitcher>(UILayout{ {32, 32}, Origin::Center }, TextureSlice::One, GetStateKeyMap("ExitButton1")), { 100, 100 });
    AttachComponentToPanel(CreateComponent<TextureSwitcher>(UILayout{ {180, 48}, Origin::Center }, TextureSlice::ThreeH, GetStateKeyMap("ScrollButton3_H")), { 400, 300 });
    AttachComponentToPanel(CreateComponent<TextureSwitcher>(UILayout{ {180, 48}, Origin::Center }, TextureSlice::ThreeH, GetStateKeyMap("ScrollButton3_H")), { 400, 240 });
    vector<wstring> bindFontKeys{ L"English", L"Hangle" };
    auto texController = GetRenderer()->GetTextureController();
    AttachComponentToPanel(CreateComponent<TextArea>(texController, UILayout{ {250, 120}, Origin::Center }, L"<Hangle>테스트 입니다!</Hangle> <English><Red>Test!</Red></English>", bindFontKeys), { 160, 420 });
    AttachComponentToPanel(CreateComponent<PatchTextureStd9>(UILayout{ {210, 150}, Origin::LeftTop }, "BackImage9"), { 400, 300 });
    AttachComponentToPanel(CreateSampleListArea({ {200, 170}, Origin::Center }), { 600, 200 });
    ReturnIfFalse(m_uiModule->BindTextureResources());

    ListArea* list = FindComponent<ListArea*>(m_uiModule->GetMainPanel(), "ListArea");
    MakeSampleListAreaData(texController, m_uiModule->GetTexResBinder(), list, 13);

    return true;
}

bool ComponentTestScene::AttachComponentToPanel(unique_ptr<UIComponent> component, const XMINT2& position) const noexcept
{
    if (!component) return false;
    AttachComponent(m_uiModule->GetMainPanel(), move(component), position);

    return true;
}

void ComponentTestScene::Update(const DX::StepTimer& timer)
{
    m_uiModule->Update(timer);
}