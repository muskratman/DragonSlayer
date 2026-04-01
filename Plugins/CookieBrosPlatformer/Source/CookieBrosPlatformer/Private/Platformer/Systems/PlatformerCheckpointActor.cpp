#include "Platformer/Systems/PlatformerCheckpointActor.h"
#include "Components/BoxComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/PlatformerCheckpointRegistry.h"
#include "Interfaces/PlatformerSaveProgressService.h"
#include "Kismet/GameplayStatics.h"

APlatformerCheckpointActor::APlatformerCheckpointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	TriggerZone->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = TriggerZone;
}

void APlatformerCheckpointActor::BeginPlay()
{
	Super::BeginPlay();
}

void APlatformerCheckpointActor::Interact_Implementation(AActor* Interactor)
{
	bIsActive = true;

	if (AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
		IsValid(GameMode) && GameMode->GetClass()->ImplementsInterface(UPlatformerCheckpointRegistry::StaticClass()))
	{
		IPlatformerCheckpointRegistry::Execute_RegisterCheckpointActor(GameMode, this);
	}

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
		IsValid(GameInstance) && GameInstance->GetClass()->ImplementsInterface(UPlatformerSaveProgressService::StaticClass()))
	{
		IPlatformerSaveProgressService::Execute_SavePlatformerProgress(GameInstance);
	}
}

bool APlatformerCheckpointActor::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

FText APlatformerCheckpointActor::GetInteractionPrompt_Implementation() const
{
	return FText::Format(NSLOCTEXT("CookieBrosPlatformer", "CheckpointPrompt", "Rest at {0}"), CheckpointName);
}
