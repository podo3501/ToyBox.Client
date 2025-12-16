#include "pch.h"
#include "TestScene2.h"
#include "IRenderer.h"
#include "Locator/SceneLocator.h"
#include "Locator/EventDispatcherLocator.h"
#include "UserInterface/UIModule.h"
#include "UserInterface/UIComponentLocator.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "TestScene1.h"

TestScene2::TestScene2(IRenderer* renderer) :
	Scene(renderer)
{}

bool TestScene2::Enter()
{
	auto texResBinder = CreateTextureResourceBinder(L"UI/SampleTexture/SampleTextureBinder.json", GetRenderer());
	m_uiModule = CreateUIModule("Test2", L"/Scene/Test/TestScene2.json", move(texResBinder));

	auto scene = SceneLocator::GetService();
	auto eventDispatcher = EventDispatcherLocator::GetService();
	eventDispatcher->Subscribe("", "TextureSwitcher", [this, scene](UIEvent event) {
		if (event == UIEvent::Clicked)
			scene->Transition(make_unique<TestScene1>(GetRenderer()));
		});

	return true;
}

bool TestScene2::Leave()
{
	auto eventDispatcher = EventDispatcherLocator::GetService();
	eventDispatcher->Clear();

	return ReleaseUIModule("Test2");
}

void TestScene2::Update(const DX::StepTimer& timer)
{
	m_uiModule->Update(timer);
}