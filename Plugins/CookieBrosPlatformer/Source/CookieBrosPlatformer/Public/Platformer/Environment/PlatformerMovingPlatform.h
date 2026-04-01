// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Platformer/Character/PlatformerInteractable.h"
#include "PlatformerMovingPlatform.generated.h"

class UTexture2D;

/**
 * Simple moving platform that can be triggered through interactions by other actors.
 * The actual movement is performed by Blueprint code through latent execution nodes.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerMovingPlatform : public AActor, public IPlatformerInteractable
{
	GENERATED_BODY()

public:
	APlatformerMovingPlatform();

protected:
	bool bMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Moving Platform")
	FVector PlatformTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Moving Platform", meta=(ClampMin=0, ClampMax=10, Units="s"))
	float MoveDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category="Moving Platform")
	bool bOneShot = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

public:
	virtual void Interaction(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	virtual void ResetInteraction();

protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Moving Platform", meta=(DisplayName="Move to Target"))
	void BP_MoveToTarget();
};
