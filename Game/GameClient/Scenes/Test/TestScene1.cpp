#include "pch.h"
#include "TestScene1.h"
#include "Renderer/Public/IRenderer.h"
#include "GameCore/Scenes/SceneLocator.h"
#include "Locator/UIComponentLocator.h"
#include "GameCore/Locator/EventDispatcherLocator.h"
#include "UserInterface/UIModule.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "TestScene2.h"
#include "Device/Storage/IJsonStorage.h"

TestScene1::~TestScene1() = default;
TestScene1::TestScene1(IRenderer* renderer) :
	m_renderer{ renderer }
{}

bool TestScene1::Enter()
{
	JsonStorageDesc storageDesc;
	storageDesc.SetFilename<StorageKey::Definition>(L"/Scene/Test/TestScene1.json");
	storageDesc.SetFilename<StorageKey::Resource>(L"UI/SampleTexture/SampleTextureBinder.json");
	auto storage = CreateJsonStorage(storageDesc);
	auto texResBinder = CreateTextureResourceBinder(storage.get(), m_renderer);
	m_uiModule = CreateUIModule("Test1", move(storage), move(texResBinder));

	//씬이 시작될때 등록하고 씬이 나갈때 해제한다.
	//여기서 하면 씬 이름까지 넣어줘야 한다.
	auto scene = SceneLocator::GetService();
	auto eventDispatcher = EventDispatcherLocator::GetService();
	eventDispatcher->Subscribe("", "TextureSwitcher", [this, scene](UIEvent event) {
		if (event == UIEvent::Clicked)
			scene->Transition(make_unique<TestScene2>(m_renderer));
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