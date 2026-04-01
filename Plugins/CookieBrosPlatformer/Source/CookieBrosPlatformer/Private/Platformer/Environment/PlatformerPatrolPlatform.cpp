#include "Platformer/Environment/PlatformerPatrolPlatform.h"

APlatformerPatrolPlatform::APlatformerPatrolPlatform()
{
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerPatrolPlatform.PlatformerPatrolPlatform")));
}

void APlatformerPatrolPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (!bAutoStart)
	{
		return;
	}

	EnterPauseState(bStartAtPointB ? EPlatformerMoverState::IdleAtPointB : EPlatformerMoverState::IdleAtPointA);
}

void APlatformerPatrolPlatform::HandlePauseFinishedAtPointA()
{
	StartMovingToPointB();
}

void APlatformerPatrolPlatform::HandlePauseFinishedAtPointB()
{
	StartMovingToPointA();
}
