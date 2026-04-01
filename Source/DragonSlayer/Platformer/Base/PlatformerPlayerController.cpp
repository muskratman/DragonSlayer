// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformerPlayerController.h"
#include "Core/UI/DragonSlayerHUD.h"
#include "Platformer/Character/PlayableDragonCharacter.h"
#include "Engine/World.h"

APawn* APlatformerPlayerController::SpawnRespawnPawn(const FTransform& SpawnTransform)
{
	if (CharacterClass && GetWorld())
	{
		return GetWorld()->SpawnActor<APlayableDragonCharacter>(CharacterClass, SpawnTransform);
	}

	return Super::SpawnRespawnPawn(SpawnTransform);
}

void APlatformerPlayerController::HandlePauseRequested()
{
	if (ADragonSlayerHUD* BurningHUD = Cast<ADragonSlayerHUD>(GetHUD()))
	{
		BurningHUD->TogglePauseMenu();
	}
}
