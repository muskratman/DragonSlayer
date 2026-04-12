#include "Platformer/Environment/PlatformerYokuBlocks.h"

#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

APlatformerYokuBlocks::APlatformerYokuBlocks()
{
	PrimaryActorTick.bCanEverTick = false;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerVanishingBlock.PlatformerVanishingBlock")));
}

void APlatformerYokuBlocks::BeginPlay()
{
	Super::BeginPlay();

	SetBlockVisibleState(false);

	if (InitialDelay > 0.0f)
	{
		ScheduleNextStateTransition(InitialDelay, &APlatformerYokuBlocks::ShowBlock);
	}
	else
	{
		ShowBlock();
	}
}

void APlatformerYokuBlocks::ShowBlock()
{
	SetBlockVisibleState(true);
	ScheduleNextStateTransition(ShowDuration, &APlatformerYokuBlocks::HideBlock);
}

void APlatformerYokuBlocks::HideBlock()
{
	SetBlockVisibleState(false);
	ScheduleNextStateTransition(HidenDuration, &APlatformerYokuBlocks::ShowBlock);
}

void APlatformerYokuBlocks::SetBlockVisibleState(bool bNewVisible)
{
	bIsBlockVisible = bNewVisible;

	if (UStaticMeshComponent* PlatformMesh = BlockMesh)
	{
		PlatformMesh->SetHiddenInGame(!bNewVisible);
		PlatformMesh->SetCollisionEnabled(bNewVisible ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}

void APlatformerYokuBlocks::ScheduleNextStateTransition(float Delay, void (APlatformerYokuBlocks::*NextStateFunction)())
{
	if (UWorld* World = GetWorld())
	{
		if (Delay <= 0.0f)
		{
			FTimerDelegate NextStateDelegate;
			NextStateDelegate.BindUObject(this, NextStateFunction);
			World->GetTimerManager().SetTimerForNextTick(NextStateDelegate);
			return;
		}

		World->GetTimerManager().SetTimer(StateTimerHandle, this, NextStateFunction, Delay, false);
	}
}
