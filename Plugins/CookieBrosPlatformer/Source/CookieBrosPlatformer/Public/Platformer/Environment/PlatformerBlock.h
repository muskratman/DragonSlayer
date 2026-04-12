#pragma once

#include "CoreMinimal.h"
#include "Platformer/Environment/PlatformerBlockBase.h"
#include "PlatformerBlock.generated.h"

class UStaticMesh;

UENUM(BlueprintType)
enum class EPlatformerBlockMeshVariant : uint8
{
	FullSize,
	HalfSize
};

/**
 * Simple cubic block with adjustable size and mesh-variant selection.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerBlock : public APlatformerBlockBase
{
	GENERATED_BODY()

public:
	APlatformerBlock();

	void SetBlockMeshVariant(EPlatformerBlockMeshVariant InBlockMeshVariant);

	FORCEINLINE EPlatformerBlockMeshVariant GetBlockMeshVariant() const { return BlockMeshVariant; }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	UStaticMesh* ResolveBlockStaticMesh() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Block|Visual")
	EPlatformerBlockMeshVariant BlockMeshVariant = EPlatformerBlockMeshVariant::FullSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Block|Visual")
	TObjectPtr<UStaticMesh> FullSizeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Block|Visual")
	TObjectPtr<UStaticMesh> HalfSizeMesh;
};
