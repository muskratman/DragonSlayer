#include "Platformer/Environment/PlatformerGravityVolume.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

APlatformerGravityVolume::APlatformerGravityVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerGravityVolume.PlatformerGravityVolume")));

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	RootComponent = Volume;

	Volume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Volume->SetCollisionObjectType(ECC_WorldDynamic);
	Volume->SetCollisionResponseToAllChannels(ECR_Ignore);
	Volume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Volume->OnComponentBeginOverlap.AddDynamic(this, &APlatformerGravityVolume::OnVolumeBeginOverlap);
	Volume->OnComponentEndOverlap.AddDynamic(this, &APlatformerGravityVolume::OnVolumeEndOverlap);
}

void APlatformerGravityVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Volume->SetBoxExtent(VolumeExtent);
}

void APlatformerGravityVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto It = AffectedCharacters.CreateIterator(); It; ++It)
	{
		ACharacter* Character = It->Key.Get();
		if (!Character)
		{
			It.RemoveCurrent();
			continue;
		}

		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			if (MaxGravityVelocity <= 0.0f)
			{
				continue;
			}

			FVector Velocity = MovementComponent->Velocity;
			if (GravityScaleOverride >= 0.0f && Velocity.Z < -MaxGravityVelocity)
			{
				Velocity.Z = -MaxGravityVelocity;
				MovementComponent->Velocity = Velocity;
			}
			else if (GravityScaleOverride < 0.0f && Velocity.Z > MaxGravityVelocity)
			{
				Velocity.Z = MaxGravityVelocity;
				MovementComponent->Velocity = Velocity;
			}
		}
	}
}

void APlatformerGravityVolume::OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || AffectedCharacters.Contains(Character))
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
	{
		FPlatformerGravityVolumeState SavedState;
		SavedState.GravityScale = MovementComponent->GravityScale;
		SavedState.MovementMode = MovementComponent->MovementMode;

		AffectedCharacters.Add(Character, SavedState);

		MovementComponent->GravityScale = GravityScaleOverride;
		if (bUseFlyingMovementMode)
		{
			MovementComponent->SetMovementMode(MOVE_Flying);
		}
	}
}

void APlatformerGravityVolume::OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		RestoreCharacter(Character);
	}
}

void APlatformerGravityVolume::RestoreCharacter(ACharacter* Character)
{
	if (!Character)
	{
		return;
	}

	const FPlatformerGravityVolumeState* SavedState = AffectedCharacters.Find(Character);
	if (!SavedState)
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
	{
		MovementComponent->GravityScale = SavedState->GravityScale;
		MovementComponent->SetMovementMode(SavedState->MovementMode);
	}

	AffectedCharacters.Remove(Character);
}
