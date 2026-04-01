#include "Platformer/Environment/PlatformerTriggeredLift.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

APlatformerTriggeredLift::APlatformerTriggeredLift()
{
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerTriggeredLift.PlatformerTriggeredLift")));

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(PlatformMesh);
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformerTriggeredLift::OnTriggerBeginOverlap);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APlatformerTriggeredLift::OnTriggerEndOverlap);
}

void APlatformerTriggeredLift::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TriggerVolume->SetBoxExtent(TriggerExtent);
	TriggerVolume->SetRelativeLocation(FVector(0.0f, 0.0f, (PlatformSize.Z * 0.5f) + TriggerExtent.Z));
}

void APlatformerTriggeredLift::HandlePauseFinishedAtPointB()
{
	if (!bAutoReturnToPointA)
	{
		return;
	}

	ClearInvalidOccupants();
	if (OccupyingCharacters.Num() == 0)
	{
		ScheduleReturnToPointA();
	}
}

void APlatformerTriggeredLift::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<ACharacter>(OtherActor))
	{
		return;
	}

	OccupyingCharacters.Add(CastChecked<ACharacter>(OtherActor));
	GetWorldTimerManager().ClearTimer(ReturnTimerHandle);

	if (IsMoving())
	{
		return;
	}

	if (IsAtPointA())
	{
		StartMovingToPointB();
	}
	else if (IsAtPointB() && bAllowManualReturnFromPointB)
	{
		StartMovingToPointA();
	}
}

void APlatformerTriggeredLift::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		OccupyingCharacters.Remove(Character);
	}

	if (bAutoReturnToPointA && IsAtPointB() && !IsMoving())
	{
		ClearInvalidOccupants();
		if (OccupyingCharacters.Num() == 0)
		{
			ScheduleReturnToPointA();
		}
	}
}

void APlatformerTriggeredLift::HandleReturnTimerElapsed()
{
	ClearInvalidOccupants();
	if (OccupyingCharacters.Num() == 0 && IsAtPointB() && !IsMoving())
	{
		StartMovingToPointA();
	}
}

void APlatformerTriggeredLift::ClearInvalidOccupants()
{
	for (auto It = OccupyingCharacters.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
		}
	}
}

void APlatformerTriggeredLift::ScheduleReturnToPointA()
{
	if (!bAutoReturnToPointA)
	{
		return;
	}

	if (AutoReturnDelay <= 0.0f)
	{
		HandleReturnTimerElapsed();
		return;
	}

	GetWorldTimerManager().SetTimer(ReturnTimerHandle, this, &APlatformerTriggeredLift::HandleReturnTimerElapsed, AutoReturnDelay, false);
}
