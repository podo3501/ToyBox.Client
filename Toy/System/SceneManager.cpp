#include "pch.h"
#include "SceneManager.h"
#include "Scenes/Scene.h"

SceneManager::~SceneManager() = default;
SceneManager::SceneManager() :
	m_currentScene{ nullptr }
{}

void SceneManager::Transition(unique_ptr<Scene> newScene)
{
	if (m_currentScene && m_currentScene->GetTypeID() == newScene->GetTypeID())
		return;

	m_pendingScene = move(newScene);
}

void SceneManager::Update(const DX::StepTimer& timer)
{
	if (m_currentScene)
		m_currentScene->Update(timer);

	if (m_pendingScene)
		DoTransition();
}

void SceneManager::DoTransition()
{
	if (!m_pendingScene)
		return;

	if (m_currentScene)
		m_currentScene->Leave();

	m_currentScene = move(m_pendingScene);
	m_currentScene->Enter();
}

/////////////////////////////////////////////////////////////////////////////

unique_ptr<ISceneManager> CreateSceneManager()
{
	return make_unique<SceneManager>();
}
