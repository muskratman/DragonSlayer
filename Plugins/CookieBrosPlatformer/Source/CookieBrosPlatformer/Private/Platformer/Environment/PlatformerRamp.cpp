#include "Platformer/Environment/PlatformerRamp.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APlatformerRamp::APlatformerRamp()
{
	PrimaryActorTick.bCanEverTick = false;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerRamp.PlatformerRamp")));

	RootComponent = Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RampMesh"));
	RampMesh->SetupAttachment(Root);
	RampMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RampMesh->SetCollisionObjectType(ECC_WorldStatic);
	RampMesh->SetCollisionResponseToAllChannels(ECR_Block);
	RampMesh->SetCanEverAffectNavigation(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		RampMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void APlatformerRamp::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RampMesh->SetRelativeLocation(FVector(0.0f, 0.0f, RampSize.Z * 0.5f));
	RampMesh->SetRelativeScale3D(RampSize / 100.0f);
	RampMesh->SetRelativeRotation(FRotator(SlopePitch, 0.0f, SlopeRoll));
	RampMesh->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Increase, 89.0f));
}
