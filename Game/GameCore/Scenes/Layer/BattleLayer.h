#pragma once
#include "SceneLayer.h"

class BattleState;
class BattleLayer : public SceneLayer
{
public:
	~BattleLayer();
	void SetState(BattleState* battleState) { m_battleState = battleState; }

private:
	BattleState* m_battleState{ nullptr };
};