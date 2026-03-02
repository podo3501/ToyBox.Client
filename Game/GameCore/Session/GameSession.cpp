#include "pch.h"
#include "GameSession.h"

unique_ptr<GameSession> CreateGameSession()
{
	return make_unique<GameSession>();
}