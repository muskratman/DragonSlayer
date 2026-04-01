// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/PlatformerGameModeBase.h"
#include "PlatformerGameMode.generated.h"

class ACharacter;
class UPlatformerUI;
class APlatformerPickup;

/**
 *  Simple platformer game mode
 *  Spawns and manages the game UI
 *  Counts pickups collected by the player
 */
UCLASS()
class APlatformerGameMode : public APlatformerGameModeBase
{
	GENERATED_BODY()
	
public:
	APlatformerGameMode();

protected:

	/** Class of UI widget to spawn when the game starts */
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UPlatformerUI> UserInterfaceClass;

	/** User interface widget for the game */
	UPROPERTY(BlueprintReadOnly, Category="UI")
	TObjectPtr<UPlatformerUI> UserInterface;

protected:

	/** Initialization */
	virtual void BeginPlay() override;
	virtual void HandlePickupCollected(APlatformerPickup* Pickup, ACharacter* Collector) override;

public:

	virtual void ProcessPickup();
};
