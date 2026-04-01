// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/PlatformerPlayerControllerBase.h"
#include "PlatformerPlayerController.generated.h"

class APlayableDragonCharacter;

/**
 *  A simple platformer player controller
 *  Manages input mappings
 *  Respawns the player pawn at the player start if it is destroyed
 */
UCLASS(Config="Game")
class APlatformerPlayerController : public APlatformerPlayerControllerBase
{
	GENERATED_BODY()

protected:

	/** Production pawn class to respawn when the possessed pawn is destroyed */
	UPROPERTY(EditAnywhere, Category="Respawn")
	TSubclassOf<APlayableDragonCharacter> CharacterClass;

protected:
	virtual APawn* SpawnRespawnPawn(const FTransform& SpawnTransform) override;
	virtual void HandlePauseRequested() override;
};
