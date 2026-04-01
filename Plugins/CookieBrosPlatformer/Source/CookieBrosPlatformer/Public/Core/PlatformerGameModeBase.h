// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/PlatformerBossEncounterRegistry.h"
#include "Interfaces/PlatformerCheckpointRegistry.h"
#include "Interfaces/PlatformerPickupSink.h"
#include "PlatformerGameModeBase.generated.h"

class ACharacter;
class APlatformerBossBase;
class APlatformerCheckpointActor;
class APlatformerPickup;

/**
 * Generic platformer game mode shell that owns checkpoint, boss encounter,
 * and pickup service contracts for runtime platformer flow.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerGameModeBase : public AGameModeBase, public IPlatformerCheckpointRegistry, public IPlatformerBossEncounterRegistry, public IPlatformerPickupSink
{
	GENERATED_BODY()

public:
	APlatformerGameModeBase();

	UFUNCTION(BlueprintCallable, Category="Flow")
	virtual void RespawnPlayerAtCheckpoint();

	UFUNCTION(BlueprintCallable, Category="Flow")
	virtual void ActivateBossEncounter(APlatformerBossBase* Boss);

	UFUNCTION(BlueprintCallable, Category="Flow")
	virtual void OnLevelCompleted();

	UFUNCTION(BlueprintCallable, Category="Checkpoint")
	void RegisterCheckpoint(APlatformerCheckpointActor* Checkpoint);

	virtual void RegisterCheckpointActor_Implementation(APlatformerCheckpointActor* Checkpoint) override;
	virtual void RegisterBossEncounterActor_Implementation(APlatformerBossBase* Boss) override;
	virtual void ProcessPlatformerPickup_Implementation(APlatformerPickup* Pickup, ACharacter* Collector) override;

	FORCEINLINE APlatformerCheckpointActor* GetLastCheckpoint() const { return LastCheckpoint; }
	FORCEINLINE int32 GetPickupsCollected() const { return PickupsCollected; }

protected:
	virtual void HandlePickupCollected(APlatformerPickup* Pickup, ACharacter* Collector);

	UPROPERTY()
	TObjectPtr<APlatformerCheckpointActor> LastCheckpoint;

	UPROPERTY(BlueprintReadOnly, Category="Pickups")
	int32 PickupsCollected = 0;
};
