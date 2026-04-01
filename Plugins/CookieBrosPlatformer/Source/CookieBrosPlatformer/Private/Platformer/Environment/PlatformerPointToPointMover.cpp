#include "Platformer/Environment/PlatformerPointToPointMover.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APlatformerPointToPointMover::APlatformerPointToPointMover()
{
	PrimaryActorTick.bCanEverTick = true;
	PaletteIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/CookieBrosPlatformer/Textures/PlatformerPointToPointMover.PlatformerPointToPointMover")));

	RootComponent = Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(Root);
	PlatformMesh->SetMobility(EComponentMobility::Movable);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlatformMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PlatformMesh->SetGenerateOverlapEvents(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PlatformMesh->SetStaticMesh(CubeMesh.Object);
	}

	PointA = CreateDefaultSubobject<UArrowComponent>(TEXT("PointA"));
	PointA->SetupAttachment(Root);
	PointA->ArrowSize = 1.2f;

	PointB = CreateDefaultSubobject<UArrowComponent>(TEXT("PointB"));
	PointB->SetupAttachment(Root);
	PointB->SetRelativeLocation(FVector(500.0f, 0.0f, 0.0f));
	PointB->ArrowSize = 1.2f;
}

void APlatformerPointToPointMover::BeginPlay()
{
	Super::BeginPlay();

	CachedPointALocation = PointA->GetComponentLocation();
	CachedPointBLocation = PointB->GetComponentLocation();

	MoveState = bStartAtPointB ? EPlatformerMoverState::IdleAtPointB : EPlatformerMoverState::IdleAtPointA;
	SetActorLocation(bStartAtPointB ? CachedPointBLocation : CachedPointALocation);
}

void APlatformerPointToPointMover::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	PlatformMesh->SetRelativeLocation(FVector(0.0f, 0.0f, PlatformSize.Z * 0.5f));
	PlatformMesh->SetRelativeScale3D(PlatformSize / 100.0f);
}

void APlatformerPointToPointMover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (MoveState)
	{
	case EPlatformerMoverState::MovingToPointB:
		MoveTowards(CachedPointBLocation, DeltaTime, EPlatformerMoverState::IdleAtPointB);
		break;

	case EPlatformerMoverState::MovingToPointA:
		MoveTowards(CachedPointALocation, DeltaTime, EPlatformerMoverState::IdleAtPointA);
		break;

	case EPlatformerMoverState::IdleAtPointA:
	case EPlatformerMoverState::IdleAtPointB:
		AdvancePause(DeltaTime);
		break;
	}
}

void APlatformerPointToPointMover::MoveTowards(const FVector& TargetLocation, float DeltaTime, EPlatformerMoverState ArrivalState)
{
	const FVector CurrentLocation = GetActorLocation();
	const FVector NextLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

	SetActorLocation(NextLocation);

	if (FVector::DistSquared(NextLocation, TargetLocation) <= FMath::Square(ArrivalTolerance))
	{
		SetActorLocation(TargetLocation);
		EnterPauseState(ArrivalState);
	}
}

void APlatformerPointToPointMover::EnterPauseState(EPlatformerMoverState NewState)
{
	MoveState = NewState;
	PauseTimer = 0.0f;

	if (NewState == EPlatformerMoverState::IdleAtPointA)
	{
		HandleReachedPointA();

		if (PauseTimeAtPointA > 0.0f)
		{
			PauseTimer = PauseTimeAtPointA;
		}
		else
		{
			HandlePauseFinishedAtPointA();
		}
	}
	else if (NewState == EPlatformerMoverState::IdleAtPointB)
	{
		HandleReachedPointB();

		if (PauseTimeAtPointB > 0.0f)
		{
			PauseTimer = PauseTimeAtPointB;
		}
		else
		{
			HandlePauseFinishedAtPointB();
		}
	}
}

void APlatformerPointToPointMover::AdvancePause(float DeltaTime)
{
	if (PauseTimer <= 0.0f)
	{
		return;
	}

	PauseTimer = FMath::Max(0.0f, PauseTimer - DeltaTime);
	if (PauseTimer > 0.0f)
	{
		return;
	}

	if (MoveState == EPlatformerMoverState::IdleAtPointA)
	{
		HandlePauseFinishedAtPointA();
	}
	else if (MoveState == EPlatformerMoverState::IdleAtPointB)
	{
		HandlePauseFinishedAtPointB();
	}
}

void APlatformerPointToPointMover::HandleReachedPointA()
{
}

void APlatformerPointToPointMover::HandleReachedPointB()
{
}

void APlatformerPointToPointMover::HandlePauseFinishedAtPointA()
{
}

void APlatformerPointToPointMover::HandlePauseFinishedAtPointB()
{
}

void APlatformerPointToPointMover::StartMovingToPointA()
{
	PauseTimer = 0.0f;
	MoveState = EPlatformerMoverState::MovingToPointA;
}

void APlatformerPointToPointMover::StartMovingToPointB()
{
	PauseTimer = 0.0f;
	MoveState = EPlatformerMoverState::MovingToPointB;
}
