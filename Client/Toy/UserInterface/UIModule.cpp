#include "pch.h"
#include "UIModule.h"
#include "InputEvent/MouseEventRouter.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "UIComponent/Traverser/UIDetailTraverser.h"
#include "UIComponent/Traverser/UITraverser.h"
#include "UINameGenerator/UINameGenerator.h"
#include "UIComponent/Components/Panel.h"
#include "Device/Storage/JsonObjectIO.h"
#include "Shared/Utils/StlExt.h"

using namespace UITraverser;
using namespace UIDetailTraverser;

UIModule::~UIModule() = default;
UIModule::UIModule(unique_ptr<IJsonStorage> storage) :
	m_storage{ move(storage) },
	m_nameGen{ make_unique<UINameGenerator>() },
	m_mouseEventRouter{ make_unique<MouseEventRouter>() }
{}

bool UIModule::operator==(const UIModule& other) const noexcept
{
	ReturnIfFalse(Compare(m_nameGen, other.m_nameGen));
	ReturnIfFalse(Compare(m_mainPanel, other.m_mainPanel));

	return true;
}

bool UIModule::SetupMainComponent(const UILayout& layout, const string& name, unique_ptr<TextureResourceBinder> resBinder)
{
	m_resBinder = move(resBinder);
	m_mainPanel = CreateComponent<Panel>(layout);
	m_mainPanel->SetUIModule(this);
	m_mouseEventRouter->SetComponent(m_mainPanel.get());
	return Rename(m_mainPanel.get(), name);
}

bool UIModule::SetupMainComponent(unique_ptr<TextureResourceBinder> resBinder)
{
	m_resBinder = move(resBinder);
	ReturnIfFalse(Read());
	ReturnIfFalse(BindTextureResources());

	return true;
}

bool UIModule::BindTextureResources() noexcept
{
	ReturnIfFalse(m_resBinder);
	ReturnIfFalse(BindTextureSourceInfo(m_mainPanel.get(), m_resBinder.get()));

	return true;
}

bool UIModule::Update(const DX::StepTimer& timer) noexcept
{
	m_mouseEventRouter->UpdateMouseState();
	return UIDetailTraverser::Update(m_mainPanel.get(), timer);
}

void UIModule::Render(ITextureRender* render) const
{
	UIDetailTraverser::Render(m_mainPanel.get(), render);
}

void UIModule::ReloadDatas() noexcept
{
	m_mainPanel->SetUIModule(this);
}

void UIModule::Serialize(Serializer& serializer)
{
	serializer.Process("MainPanel", m_mainPanel);
	serializer.Process("UINameGenerator", m_nameGen);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}

bool UIModule::Write(const wstring& filename) noexcept
{
	if (!filename.empty())
	{
		auto desc = m_storage->GetDescription();
		desc->SetFilename<StorageKey::Definition>(filename);
	}

	JsonObjectIO::Write<StorageKey::Definition>(*this, m_storage.get());
	return true;
}

bool UIModule::Read() noexcept
{
	JsonObjectIO::Read<StorageKey::Definition>(*this, m_storage.get());
	PropagateRoot(m_mainPanel.get()); //모든 컴포넌트들에 root를 지정.
	m_mouseEventRouter->SetComponent(m_mainPanel.get());

	return true;
}

bool UIModule::EnableToolMode(bool enable) noexcept
{
	return UIDetailTraverser::EnableToolMode(m_mainPanel.get(), enable);
}

wstring UIModule::GetFilename() const noexcept 
{ 
	auto desc = m_storage->GetDescription();
	return desc->GetFilename<StorageKey::Definition>();
}

Panel* UIModule::GetMainPanel() const noexcept { return m_mainPanel.get(); }