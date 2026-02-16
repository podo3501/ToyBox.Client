#include "pch.h"
#include "TestScene2.h"
#include "Renderer/Public/IRenderer.h"
#include "GameCore/Locator/SceneLocator.h"
#include "Locator/UIComponentLocator.h"
#include "GameCore/Locator/EventDispatcherLocator.h"
#include "UserInterface/UIModule.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "TestScene1.h"

TestScene2::TestScene2(IRenderer* renderer) :
	m_renderer{ renderer }
{}

bool TestScene2::Enter()
{
	JsonStorageDesc storageDesc;
	storageDesc.SetFilename<StorageKey::Definition>(L"/Scene/Test/TestScene2.json");
	storageDesc.SetFilename<StorageKey::Resource>(L"UI/SampleTexture/SampleTextureBinder.json");
	auto storage = CreateJsonStorage(storageDesc);
	auto texResBinder = CreateTextureResourceBinder(storage.get(), m_renderer);
	m_uiModule = CreateUIModule("Test2", move(storage), move(texResBinder));

	auto scene = SceneLocator::GetService();
	auto eventDispatcher = EventDispatcherLocator::GetService();
	eventDispatcher->Subscribe("", "TextureSwitcher", [this, scene](UIEvent event) {
		if (event == UIEvent::Clicked)
			scene->Transition(make_unique<TestScene1>(m_renderer));
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