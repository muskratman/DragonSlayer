// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerJumpPad.generated.h"

class UBoxComponent;
class UTexture2D;

/**
 * A simple platformer jump pad that launches characters into the air.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerJumpPad : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoxComponent> Box;

protected:
	UPROPERTY(EditAnywhere, Category="Jump Pad", meta=(ClampMin=0, ClampMax=10000, Units="cm/s"))
	float ZStrength = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

public:
	APlatformerJumpPad();

protected:
	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
