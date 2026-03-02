#pragma once
#include "SceneState.h"

class GameSession;
class BattleState : public SceneState
{
public:
	~BattleState();
	explicit BattleState(GameSession* gameSession);

private:
	GameSession* m_gameSession{ nullptr };
};
