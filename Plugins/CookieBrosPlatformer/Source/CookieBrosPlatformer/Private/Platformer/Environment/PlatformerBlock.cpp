#include "Platformer/Environment/PlatformerBlock.h"

#include "UObject/ConstructorHelpers.h"

APlatformerBlock::APlatformerBlock()
{
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerBlock.PlatformerBlock")));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		FullSizeMesh = CubeMesh.Object;
	}
}

void APlatformerBlock::SetBlockMeshVariant(EPlatformerBlockMeshVariant InBlockMeshVariant)
{
	BlockMeshVariant = InBlockMeshVariant;
	if (UStaticMesh* ResolvedBlockMesh = ResolveBlockStaticMesh())
	{
		BlockMesh->SetStaticMesh(ResolvedBlockMesh);
	}
	RefreshBlockLayout();
}

UStaticMesh* APlatformerBlock::ResolveBlockStaticMesh() const
{
	switch (BlockMeshVariant)
	{
	case EPlatformerBlockMeshVariant::HalfSize:
		return HalfSizeMesh != nullptr ? HalfSizeMesh : FullSizeMesh;

	case EPlatformerBlockMeshVariant::FullSize:
	default:
		return FullSizeMesh;
	}
}

void APlatformerBlock::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (UStaticMesh* ResolvedBlockMesh = ResolveBlockStaticMesh())
	{
		BlockMesh->SetStaticMesh(ResolvedBlockMesh);
	}
}
