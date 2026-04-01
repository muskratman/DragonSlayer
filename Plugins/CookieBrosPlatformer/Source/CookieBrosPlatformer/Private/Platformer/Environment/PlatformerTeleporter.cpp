#include "Platformer/Environment/PlatformerTeleporter.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

APlatformerTeleporter::APlatformerTeleporter()
{
	PrimaryActorTick.bCanEverTick = false;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerTeleporter.PlatformerTeleporter")));

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;

	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformerTeleporter::OnTriggerVolumeBeginOverlap);

	ExitPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitPoint"));
	ExitPoint->SetupAttachment(RootComponent);
	ExitPoint->SetRelativeLocation(FVector(150.0f, 0.0f, 0.0f));
}

void APlatformerTeleporter::RegisterArrival(AActor* Actor)
{
	MarkActorTeleported(Actor);
}

void APlatformerTeleporter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	TriggerVolume->SetBoxExtent(TriggerExtent);
}

void APlatformerTeleporter::OnTriggerVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !DestinationTeleporter || DestinationTeleporter == this || !CanTeleportActor(OtherActor))
	{
		return;
	}

	DestinationTeleporter->RegisterArrival(OtherActor);
	MarkActorTeleported(OtherActor);

	OtherActor->TeleportTo(DestinationTeleporter->GetExitLocation(), DestinationTeleporter->GetExitRotation(), false, true);

	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			if (!DestinationTeleporter->bKeepVelocity)
			{
				MovementComponent->StopMovementImmediately();
			}

			if (DestinationTeleporter->ExitSpeed > 0.0f)
			{
				MovementComponent->Velocity = DestinationTeleporter->GetExitRotation().Vector() * DestinationTeleporter->ExitSpeed;
			}
		}
	}
}

bool APlatformerTeleporter::CanTeleportActor(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	if (const float* LastTeleportTime = RecentlyTeleportedActors.Find(Actor))
	{
		return (GetWorld()->GetTimeSeconds() - *LastTeleportTime) >= TeleportCooldown;
	}

	return true;
}

void APlatformerTeleporter::MarkActorTeleported(AActor* Actor)
{
	if (Actor)
	{
		RecentlyTeleportedActors.Add(Actor, GetWorld()->GetTimeSeconds());
	}
}

FVector APlatformerTeleporter::GetExitLocation() const
{
	return ExitPoint->GetComponentLocation();
}

FRotator APlatformerTeleporter::GetExitRotation() const
{
	return ExitPoint->GetComponentRotation();
}
