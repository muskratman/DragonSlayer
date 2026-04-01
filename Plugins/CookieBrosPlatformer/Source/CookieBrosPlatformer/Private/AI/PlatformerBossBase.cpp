#include "AI/PlatformerBossBase.h"

#include "GameFramework/GameModeBase.h"
#include "Interfaces/PlatformerBossEncounterRegistry.h"
#include "Kismet/GameplayStatics.h"

APlatformerBossBase::APlatformerBossBase()
{
	// Bosses generally have much larger stagger thresholds or are immune to basic hit reactions.
	StaggerThreshold = 50.0f;
}

void APlatformerBossBase::BeginPlay()
{
	Super::BeginPlay();

	if (AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
		IsValid(GameMode) && GameMode->GetClass()->ImplementsInterface(UPlatformerBossEncounterRegistry::StaticClass()))
	{
		IPlatformerBossEncounterRegistry::Execute_RegisterBossEncounterActor(GameMode, this);
	}
}

void APlatformerBossBase::TransitionToPhase_Implementation(int32 NextPhase)
{
	CurrentPhase = NextPhase;
}
