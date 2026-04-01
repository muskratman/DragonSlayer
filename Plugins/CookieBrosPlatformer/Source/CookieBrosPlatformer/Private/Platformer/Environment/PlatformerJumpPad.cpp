// Copyright Epic Games, Inc. All Rights Reserved.

#include "Platformer/Environment/PlatformerJumpPad.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

APlatformerJumpPad::APlatformerJumpPad()
{
	PrimaryActorTick.bCanEverTick = false;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerJumpPad.PlatformerJumpPad")));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Box->SetBoxExtent(FVector(115.0f, 90.0f, 20.0f), false);
	Box->SetRelativeLocation(FVector(0.0f, 0.0f, 16.0f));
	Box->SetCollisionObjectType(ECC_WorldDynamic);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &APlatformerJumpPad::BeginOverlap);
}

void APlatformerJumpPad::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor))
	{
		OverlappingCharacter->Jump();

		const FVector LaunchVelocity = FVector::UpVector * ZStrength;
		OverlappingCharacter->LaunchCharacter(LaunchVelocity, false, true);
	}
}
