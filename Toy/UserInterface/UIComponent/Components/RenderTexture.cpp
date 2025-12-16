#include "pch.h"
#include "RenderTexture.h"
#include "IRenderer.h"
#include "Locator/InputLocator.h"
#include "../../UIComponent/Traverser/UITraverser.h"
#include "../../UIComponentLocator.h"
#include "Shared/Utils/GeometryExt.h"
#include "Shared/SerializerIO/SerializerIO.h"

using namespace UITraverser;

RenderTexture::~RenderTexture() 
{ 
	Release(); 
}

RenderTexture::RenderTexture() :
	m_component{ nullptr }
{}

RenderTexture::RenderTexture(const RenderTexture& o) :
	UIComponent{ o },
	m_component{ nullptr },
	m_index{ o.m_index }
{
	ReloadDatas();
}

void RenderTexture::Release() noexcept
{
	if (!m_index) return;
	
	auto componentManager = UIComponentLocator::GetService();
	componentManager->ReleaseRenderTexture(*m_index);

	m_gfxOffset = {};
	m_index = nullopt;
}

unique_ptr<UIComponent> RenderTexture::CreateClone() const
{
	return unique_ptr<RenderTexture>(new RenderTexture(*this));
}

void RenderTexture::ReloadDatas() noexcept
{
	vector<UIComponent*> componentList = GetChildren();
	m_component = componentList[0];
}

bool RenderTexture::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));

	const RenderTexture* o = static_cast<const RenderTexture*>(&rhs);
	ReturnIfFalse(EqualComponent(m_component, o->m_component));

	return true;
}

//?!? rendertexture를 등록하는 것을 bind를 통해서 넣으면 텍스쳐 관리를 한군데에 몰아서 할 수 있을꺼 같은데.
bool RenderTexture::BindSourceInfo(TextureResourceBinder*) noexcept
{
	if (GetSize() == XMUINT2{})
		SetSize(UITraverser::GetChildrenBoundsSize(this));
	
	return CreateComponentTexture();
}

bool RenderTexture::CreateComponentTexture()
{
	size_t index{ 0 };
	UpdatePositionsManually(this, true);

	auto componentManager = UIComponentLocator::GetService();
	ReturnIfFalse(componentManager->CreateRenderTexture(m_component, GetArea(), index, &m_gfxOffset));
	m_index = index;

	return true;
}

bool RenderTexture::ChangeSize(const XMUINT2& size, bool isForce) noexcept
{
	return UITraverser::ChangeSize(m_component, size, isForce);
}

bool RenderTexture::Setup(const UILayout& layout, unique_ptr<UIComponent> component) noexcept
{
	SetLayout(layout);
	m_component = component.get();
	AttachComponent(move(component), {});

	return true;
}

bool RenderTexture::Setup(unique_ptr<UIComponent> component) noexcept
{
	UILayout layout{ component->GetSize() };
	return Setup(layout, move(component));
}

void RenderTexture::Render(ITextureRender* render) const
{
	const auto& size = GetSize();
	RECT source{ 0, 0, static_cast<long>(size.x), static_cast<long>(size.y) };
	render->Render(m_index.value(), GetArea(), &source);

	return;
}

void RenderTexture::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}