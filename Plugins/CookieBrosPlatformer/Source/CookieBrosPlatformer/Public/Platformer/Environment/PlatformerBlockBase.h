#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Platformer/Environment/PlatformerComponentTransformOverride.h"
#include "PlatformerBlockBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTexture2D;

/**
 * Shared block-shaped environment foundation with size-driven layout.
 * Mesh variant selection lives in specialized concrete actors, not here.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerBlockBase : public AActor
{
	GENERATED_BODY()

public:
	APlatformerBlockBase();

	void SetBlockSize(const FVector& InBlockSize);

	FORCEINLINE const FVector& GetBlockSize() const { return BlockSize; }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void RefreshBlockLayout();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> BlockMeshLayoutRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> BlockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Block|Shape")
	FVector BlockSize = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Block|Components")
	FPlatformerComponentTransformOffset BlockMeshTransformOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;
};
