#include "Platformer/Environment/PlatformerBlock.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APlatformerBlock::APlatformerBlock()
{
	PrimaryActorTick.bCanEverTick = false;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerBlock.PlatformerBlock")));

	RootComponent = Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetupAttachment(Root);
	BlockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockMesh->SetCollisionObjectType(ECC_WorldStatic);
	BlockMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BlockMesh->SetCanEverAffectNavigation(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BlockMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void APlatformerBlock::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BlockMesh->SetRelativeLocation(FVector(0.0f, 0.0f, BlockSize.Z * 0.5f));
	BlockMesh->SetRelativeScale3D(BlockSize / 100.0f);
}
