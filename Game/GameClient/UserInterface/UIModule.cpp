#include "pch.h"
#include "UIModule.h"
#include "InputEvent/MouseEventRouter.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "UIComponent/Traverser/UIDetailTraverser.h"
#include "UIComponent/Traverser/UITraverser.h"
#include "UINameGenerator/UINameGenerator.h"
#include "UIComponent/Components/Panel.h"
#include "Device/Storage/JsonObjectIO.h"
#include "Core/Utils/StlExt.h"

using namespace UITraverser;
using namespace UIDetailTraverser;

UIModule::~UIModule() = default;
UIModule::UIModule(IResourceManager* resManager) :
	m_resManager{ resManager },
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

bool UIModule::SetupMainComponent(const filesystem::path& filename, unique_ptr<TextureResourceBinder> resBinder)
{
	m_resBinder = move(resBinder);
	ReturnIfFalse(Read(filename));
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

bool UIModule::Read(const filesystem::path filename) noexcept
{
	JsonObjectIO::Read(*this, filename, m_resManager);
	PropagateRoot(m_mainPanel.get()); //모든 컴포넌트들에 root를 지정.
	m_mouseEventRouter->SetComponent(m_mainPanel.get());
	m_filename = filename;

	return true;
}

bool UIModule::Write(const filesystem::path& filename) noexcept
{
	ReturnIfFalse(JsonObjectIO::Write(*this, filename, m_resManager));
	m_filename = filename;

	return true;
}

bool UIModule::EnableToolMode(bool enable) noexcept
{
	return UIDetailTraverser::EnableToolMode(m_mainPanel.get(), enable);
}

wstring UIModule::GetFilename() const noexcept 
{ 
	return m_filename;
}

Panel* UIModule::GetMainPanel() const noexcept { return m_mainPanel.get(); }