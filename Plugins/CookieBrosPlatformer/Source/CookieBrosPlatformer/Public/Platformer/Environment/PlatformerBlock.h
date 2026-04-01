#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerBlock.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTexture2D;

/**
 * Simple cubic block with adjustable size for platformer blockout use.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerBlock : public AActor
{
	GENERATED_BODY()

public:
	APlatformerBlock();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> BlockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Block|Shape")
	FVector BlockSize = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;
};
