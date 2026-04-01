// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/PlatformerGameModeBase.h"

#include "AI/PlatformerBossBase.h"
#include "Platformer/Systems/PlatformerCheckpointActor.h"

APlatformerGameModeBase::APlatformerGameModeBase()
{
}

void APlatformerGameModeBase::RespawnPlayerAtCheckpoint()
{
	UE_LOG(LogTemp, Log, TEXT("Respawn requested at the active checkpoint."));
}

void APlatformerGameModeBase::ActivateBossEncounter(APlatformerBossBase* Boss)
{
	const FString BossName = IsValid(Boss) ? Boss->GetName() : TEXT("None");
	UE_LOG(LogTemp, Log, TEXT("Boss encounter activated: %s"), *BossName);
}

void APlatformerGameModeBase::OnLevelCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("Level completed."));
}

void APlatformerGameModeBase::RegisterCheckpoint(APlatformerCheckpointActor* Checkpoint)
{
	RegisterCheckpointActor_Implementation(Checkpoint);
}

void APlatformerGameModeBase::RegisterCheckpointActor_Implementation(APlatformerCheckpointActor* Checkpoint)
{
	LastCheckpoint = Checkpoint;
}

void APlatformerGameModeBase::RegisterBossEncounterActor_Implementation(APlatformerBossBase* Boss)
{
	ActivateBossEncounter(Boss);
}

void APlatformerGameModeBase::ProcessPlatformerPickup_Implementation(APlatformerPickup* Pickup, ACharacter* Collector)
{
	++PickupsCollected;
	HandlePickupCollected(Pickup, Collector);
}

void APlatformerGameModeBase::HandlePickupCollected(APlatformerPickup* Pickup, ACharacter* Collector)
{
}
