#include "pch.h"
#include "TestScene1.h"
#include "IRenderer.h"
#include "Locator/SceneLocator.h"
#include "Locator/EventDispatcherLocator.h"
#include "UserInterface/UIModule.h"
#include "UserInterface/UIComponentLocator.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "TestScene2.h"

TestScene1::~TestScene1() = default;
TestScene1::TestScene1(IRenderer* renderer) :
	Scene(renderer)
{}

bool TestScene1::Enter()
{
	auto texResBinder = CreateTextureResourceBinder(L"UI/SampleTexture/SampleTextureBinder.json", GetRenderer());
	m_uiModule = CreateUIModule("Test1", L"/Scene/Test/TestScene1.json", move(texResBinder));

	//씬이 시작될때 등록하고 씬이 나갈때 해제한다.
	//여기서 하면 씬 이름까지 넣어줘야 한다.
	auto scene = SceneLocator::GetService();
	auto eventDispatcher = EventDispatcherLocator::GetService();
	eventDispatcher->Subscribe("", "TextureSwitcher", [this, scene](UIEvent event) {
		if (event == UIEvent::Clicked)
			scene->Transition(make_unique<TestScene2>(GetRenderer()));
		});

	return true;
}

bool TestScene1::Leave()
{
	auto eventDispatcher = EventDispatcherLocator::GetService();
	eventDispatcher->Clear();
	
	return ReleaseUIModule("Test1");
}

void TestScene1::Update(const DX::StepTimer& timer)
{
	m_uiModule->Update(timer);
}