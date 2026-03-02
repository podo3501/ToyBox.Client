#include "pch.h"
#include "BattleState.h"

BattleState::~BattleState() = default;
BattleState::BattleState(GameSession* gameSession) :
	m_gameSession{ gameSession }
{}

