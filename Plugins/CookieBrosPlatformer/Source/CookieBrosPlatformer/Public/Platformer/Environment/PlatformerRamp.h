#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerRamp.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTexture2D;

/**
 * Simple sloped ramp block with adjustable size and incline for blockout use.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerRamp : public AActor
{
	GENERATED_BODY()

public:
	APlatformerRamp();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> RampMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ramp|Shape")
	FVector RampSize = FVector(300.0f, 200.0f, 50.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ramp|Shape", meta=(ClampMin=-89.0, ClampMax=89.0, Units="deg"))
	float SlopePitch = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ramp|Shape", meta=(ClampMin=-89.0, ClampMax=89.0, Units="deg"))
	float SlopeRoll = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;
};
