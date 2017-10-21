#include "Time.hpp"

namespace gv
{
struct GameplayTimeManager
{
	// The difference between gameplay time and world time. This makes sure gameplay time stays
	// ignorant of pausing
	float GameplayTimeWorldTimeDelta;

	bool IsPlaying;
	float PausedAtWorldTime;
};

static GameplayTimeManager s_GameplayTimeManager;

float GetGameplayTime()
{
	if (s_GameplayTimeManager.IsPlaying)
	{
		return GetWorldTime() - s_GameplayTimeManager.GameplayTimeWorldTimeDelta;
	}
	else
	{
		// If we're paused, we want to return the same time regardless of world time
		return s_GameplayTimeManager.PausedAtWorldTime -
		       s_GameplayTimeManager.GameplayTimeWorldTimeDelta;
	}
}

bool GameIsPlaying()
{
	return s_GameplayTimeManager.IsPlaying;
}

void GameSetPlaying(bool shouldPlay)
{
	// Pausing
	if (!shouldPlay && s_GameplayTimeManager.IsPlaying)
	{
		s_GameplayTimeManager.PausedAtWorldTime = GetWorldTime();
	}
	// Resuming
	else if (shouldPlay && !s_GameplayTimeManager.IsPlaying)
	{
		float thisPauseTimeDelta =
		    GetWorldTime() - s_GameplayTimeManager.GameplayTimeWorldTimeDelta;
		s_GameplayTimeManager.GameplayTimeWorldTimeDelta += thisPauseTimeDelta;
	}

	s_GameplayTimeManager.IsPlaying = shouldPlay;
}

void ResetGameplayTime()
{
	s_GameplayTimeManager.GameplayTimeWorldTimeDelta = 0.f;
	s_GameplayTimeManager.IsPlaying = true;
	s_GameplayTimeManager.PausedAtWorldTime = 0.f;
}
}