#include "Platformer/Environment/PlatformerLadder.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"

APlatformerLadder::APlatformerLadder()
{
	PrimaryActorTick.bCanEverTick = false;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerLadder.PlatformerLadder")));

	RootComponent = Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(Root);
	LadderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		LadderMesh->SetStaticMesh(CubeMesh.Object);
	}

	ClimbVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbVolume"));
	ClimbVolume->SetupAttachment(Root);
	ClimbVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClimbVolume->SetCollisionObjectType(ECC_WorldDynamic);
	ClimbVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClimbVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ClimbVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformerLadder::OnClimbVolumeBeginOverlap);
	ClimbVolume->OnComponentEndOverlap.AddDynamic(this, &APlatformerLadder::OnClimbVolumeEndOverlap);
}

void APlatformerLadder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	LadderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, LadderSize.Z * 0.5f));
	LadderMesh->SetRelativeScale3D(LadderSize / 100.0f);

	ClimbVolume->SetRelativeLocation(FVector(0.0f, 0.0f, LadderSize.Z * 0.5f));
	ClimbVolume->SetBoxExtent(LadderSize * 0.5f);
}

void APlatformerLadder::OnClimbVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || ClimbingCharacters.Contains(Character))
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
	{
		FPlatformerLadderState SavedState;
		SavedState.GravityScale = MovementComponent->GravityScale;
		SavedState.MovementMode = MovementComponent->MovementMode;

		ClimbingCharacters.Add(Character, SavedState);

		MovementComponent->GravityScale = ClimbGravityScale;
		if (bUseFlyingMovementMode)
		{
			MovementComponent->SetMovementMode(MOVE_Flying);
		}

		if (bSnapCharacterDepthToLadder)
		{
			FVector CharacterLocation = Character->GetActorLocation();
			CharacterLocation.Y = GetActorLocation().Y;
			Character->SetActorLocation(CharacterLocation);
		}
	}
}

void APlatformerLadder::OnClimbVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		RestoreCharacter(Character);
	}
}

void APlatformerLadder::RestoreCharacter(ACharacter* Character)
{
	if (!Character)
	{
		return;
	}

	const FPlatformerLadderState* SavedState = ClimbingCharacters.Find(Character);
	if (!SavedState)
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
	{
		MovementComponent->GravityScale = SavedState->GravityScale;
		MovementComponent->SetMovementMode(SavedState->MovementMode);
	}

	ClimbingCharacters.Remove(Character);
}
