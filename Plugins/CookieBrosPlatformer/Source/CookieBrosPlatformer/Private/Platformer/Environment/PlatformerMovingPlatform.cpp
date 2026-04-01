// Copyright Epic Games, Inc. All Rights Reserved.

#include "Platformer/Environment/PlatformerMovingPlatform.h"
#include "Components/SceneComponent.h"

APlatformerMovingPlatform::APlatformerMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = false;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerMovingPlatform.PlatformerMovingPlatform")));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void APlatformerMovingPlatform::Interaction(AActor* Interactor)
{
	if (bMoving)
	{
		return;
	}

	bMoving = true;
	BP_MoveToTarget();
}

void APlatformerMovingPlatform::ResetInteraction()
{
	if (bOneShot)
	{
		return;
	}

	bMoving = false;
}
