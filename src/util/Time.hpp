#pragma once

namespace gv
{
// Get the time for any gameplay purposes. This is affected by pausing/time step scaling.
float GetGameplayTime();

bool GameIsPlaying();

// If shouldPlay = false gameplay is paused
void GameSetPlaying(bool shouldPlay);

void ResetGameplayTime();

// @Latelink: GetWorldTime() is a front-end specific function. It should return the amount of time
// since the game/world started. This value should NOT respect any Gameplay timing or pausing
// This should not be used for gameplay. Use GetGameplayTime() instead
float GetWorldTime();

}