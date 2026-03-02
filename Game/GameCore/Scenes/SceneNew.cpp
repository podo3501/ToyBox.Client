#include "pch.h"
#include "SceneNew.h"
#include "State/SceneState.h"
#include "Layer/SceneLayer.h"

SceneNew::~SceneNew() = default;
SceneNew::SceneNew(unique_ptr<SceneState> sceneState, unique_ptr<SceneLayer> sceneLayer) :
	m_sceneState{ move(sceneState) },
	m_sceneLayer{ move(sceneLayer) }
{}