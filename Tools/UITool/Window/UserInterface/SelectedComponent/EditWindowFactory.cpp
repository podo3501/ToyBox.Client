#include "pch.h"
#include "EditWindowFactory.h"
#include "ComponentEdit/EditPatchTexture/EditPatchTextureStd.h"
#include "ComponentEdit/EditPatchTexture/EditPatchTextureLite.h"
#include "ComponentEdit/EditButton.h"
#include "ComponentEdit/EditTextArea.h"
#include "ComponentEdit/EditContainer.h"
#include "ComponentEdit/EditRenderTexture.h"
#include "ComponentEdit/EditListArea.h"
#include "ComponentEdit/EditScrollBar.h"
#include "ComponentEdit/EditTextureSwitcher.h"
#include "Toy/UserInterface/UIComponent/Components/Panel.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd3.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd9.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite1.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite3.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite9.h"
#include "Toy/UserInterface/UIComponent/Components/Button.h"
#include "Toy/UserInterface/UIComponent/Components/TextArea.h"
#include "Toy/UserInterface/UIComponent/Components/RenderTexture.h"
#include "Toy/UserInterface/UIComponent/Components/Container.h"
#include "Toy/UserInterface/UIComponent/Components/ListArea.h"
#include "Toy/UserInterface/UIComponent/Components/ScrollBar.h"
#include "Toy/UserInterface/UIComponent/Components/TextureSwitcher.h"

using FactoryFunction = function<unique_ptr<EditWindow>(UIComponent*, UICommandHistory*, TextureResourceBinder*)>;
optional<unordered_map<ComponentID, FactoryFunction>> EditWindowFactory::m_factoryMap{ nullopt };

void EditWindowFactory::RegisterFactories()
{
    if (m_factoryMap) return; //여러번 호출해도 한번만 등록이 되게끔 nullopt(값이 없을때)만 되게끔 한다.
    m_factoryMap.emplace(unordered_map<ComponentID, FactoryFunction>()); //빈 상태로 초기값을 만든다.

    //UIComponent*, UICommandHistory*가 인자로 들어가는 새로운 EditWindow는 여기서 추가
    RegisterEditWindow<EditPanel, Panel*>(ComponentID::Panel);
    RegisterEditWindow<EditButton, Button*>(ComponentID::Button);
    RegisterEditWindow<EditContainer, Container*>(ComponentID::Container);
    RegisterEditWindow<EditListArea, ListArea*>(ComponentID::ListArea);
    RegisterEditWindow<EditPatchTextureLite1, PatchTextureLite1*>(ComponentID::PatchTextureLite1);
    RegisterEditWindow<EditPatchTextureLite3, PatchTextureLite3*>(ComponentID::PatchTextureLite3);
    RegisterEditWindow<EditPatchTextureLite9, PatchTextureLite9*>(ComponentID::PatchTextureLite9);
    RegisterEditWindow<EditRenderTexture, RenderTexture*>(ComponentID::RenderTexture);
    RegisterEditWindow<EditScrollBar, ScrollBar*>(ComponentID::ScrollBar);
    RegisterEditWindow<EditTextArea, TextArea*>(ComponentID::TextArea);

    //UIComponent*, UICommandHistory*, TextureResourceBinder*가 인자로 들어가는 새로운 EditWindow는 여기서 추가
    RegisterEditWindowWithBinder<EditPatchTextureStd1, PatchTextureStd1*>(ComponentID::PatchTextureStd1);
    RegisterEditWindowWithBinder<EditPatchTextureStd3, PatchTextureStd3*>(ComponentID::PatchTextureStd3);
    RegisterEditWindowWithBinder<EditPatchTextureStd9, PatchTextureStd9*>(ComponentID::PatchTextureStd9);
    RegisterEditWindowWithBinder<EditTextureSwitcher, TextureSwitcher*>(ComponentID::TextureSwitcher);

    //다른 인자를 더 넣게 된다면 RegisterEditWindow에 가변인자를 넣게 만들어서 RegisterEditWindow 함수를 하나로 만들자.
}

unique_ptr<EditWindow> EditWindowFactory::CreateEditWindow(UIComponent* component, 
    UICommandHistory* cmdHistory, TextureResourceBinder* resBinder)
{
    if (!component) return nullptr;

    ComponentID id = component->GetTypeID();
    auto it = m_factoryMap->find(id);
    if (it != m_factoryMap->end())
        return it->second(component, cmdHistory, resBinder);

    return nullptr;
}