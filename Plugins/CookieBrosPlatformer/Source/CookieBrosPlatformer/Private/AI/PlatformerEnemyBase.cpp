#include "AI/PlatformerEnemyBase.h"

#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StateTreeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/PlatformerEnemyArchetypeAsset.h"
#include "EngineUtils.h"
#include "GAS/Attributes/PlatformerCharacterAttributeSet.h"
#include "GAS/PlatformerGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const FRotator EnemyMeshDefaultRotation(0.0f, 90.0f, 0.0f);
	const FRotator EnemyMeshFacingRightRotation(0.0f, -90.0f, 0.0f);
	const FRotator EnemyMeshFacingLeftRotation(0.0f, 90.0f, 0.0f);

	bool IsUsingClassDefaultFloat(float CurrentValue, float ClassDefaultValue)
	{
		return FMath::IsNearlyEqual(CurrentValue, ClassDefaultValue);
	}
}

APlatformerEnemyBase::APlatformerEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	if (AttributeSet)
	{
		AttributeSet->InitMaxHealth(Health);
		AttributeSet->InitHealth(Health);
		AttributeSet->InitBaseDamage(Damage);
		AttributeSet->InitAttackSpeed(HitDelay > 0.0f ? 1.0f / HitDelay : 0.0f);
		AttributeSet->InitMeleeAttackDamage(Damage);
		AttributeSet->InitRangeBaseAttackDamage(Damage);
		AttributeSet->InitMeleeAttackDelay(HitDelay);
		AttributeSet->InitRangeAttackDelay(HitDelay);
		AttributeSet->InitMoveSpeed(MovementSpeed);
	}

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 180.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	PerceptionComponent->ConfigureSense(*SightConfig);

	DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	DamageSenseConfig->SetMaxAge(5.0f);
	PerceptionComponent->ConfigureSense(*DamageSenseConfig);
	PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &APlatformerEnemyBase::HandleTargetPerceptionUpdated);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->MaxFlySpeed = MovementSpeed;

	ApplyDefaultMeshFacing();

#if WITH_EDITORONLY_DATA
	PatrolPathSpline = CreateEditorOnlyDefaultSubobject<USplineComponent>(TEXT("PatrolPathSpline"));
	if (PatrolPathSpline)
	{
		PatrolPathSpline->SetupAttachment(RootComponent);
		PatrolPathSpline->SetClosedLoop(false);
		PatrolPathSpline->SetHiddenInGame(true);
		PatrolPathSpline->SetIsVisualizationComponent(true);
		PatrolPathSpline->bIsEditorOnly = true;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		PatrolPointPreviewMeshAsset = SphereMesh.Object;
	}
#endif

	UpdateHealthWidgetPlacement();
}

void APlatformerEnemyBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyDefaultMeshFacing();

#if WITH_EDITORONLY_DATA
	RefreshEditorPatrolPreviewComponents();
#endif
}

void APlatformerEnemyBase::InitializeFromArchetype(const UPlatformerEnemyArchetypeAsset* Archetype)
{
	if (!Archetype || !AbilitySystemComponent || !AttributeSet)
	{
		return;
	}

	const float ResolvedBaseHealth =
		(Archetype->BaseHealth <= 0.0f || FMath::IsNearlyEqual(Archetype->BaseHealth, 3.0f))
			? GetDefaultMaxHealth()
			: Archetype->BaseHealth;

	const APlatformerEnemyBase* ClassDefaults = GetClass()->GetDefaultObject<APlatformerEnemyBase>();
	if (!ClassDefaults || IsUsingClassDefaultFloat(Health, ClassDefaults->Health))
	{
		Health = FMath::Max(ResolvedBaseHealth, 1.0f);
	}

	if (!ClassDefaults || IsUsingClassDefaultFloat(Damage, ClassDefaults->Damage))
	{
		Damage = FMath::Max(Archetype->BaseDamage, 0.0f);
	}

	if (!ClassDefaults || IsUsingClassDefaultFloat(HitDelay, ClassDefaults->HitDelay))
	{
		HitDelay = Archetype->AttackSpeed > 0.0f ? 1.0f / Archetype->AttackSpeed : 0.0f;
	}

	if (!ClassDefaults || IsUsingClassDefaultFloat(MovementSpeed, ClassDefaults->MovementSpeed))
	{
		MovementSpeed = FMath::Max(Archetype->MoveSpeed, 1.0f);
	}

	CombatEngageRange = FMath::Max(Archetype->CombatEngageRange, 0.0f);
	CombatLoseTargetRange = FMath::Max(Archetype->CombatLoseTargetRange, CombatEngageRange);
	CombatAttackRange = FMath::Max(Archetype->CombatAttackRange, 0.0f);
	AttackDamageEffectClass = Archetype->DamageEffectClass;

	ApplyEnemyRuntimeSettings();

	if (SightConfig)
	{
		SightConfig->SightRadius = CombatEngageRange;
		SightConfig->LoseSightRadius = CombatLoseTargetRange;
	}

	if (PerceptionComponent)
	{
		PerceptionComponent->RequestStimuliListenerUpdate();
	}

	ApplyArchetypeCombatData(Archetype);

	if (StateTreeComponent && Archetype->BehaviorTree)
	{
		StateTreeComponent->SetStateTree(Archetype->BehaviorTree);
		StateTreeComponent->StartLogic();
	}

	if (HasAuthority())
	{
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : Archetype->Abilities)
		{
			if (AbilityClass)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
			}
		}
	}

	SyncCombatLifeStateFromAttributes();
	RefreshHealthWidget();
}

void APlatformerEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	PatrolOriginLocation = GetActorLocation();
	CurrentPatrolPointIndex = 0;
	PatrolDirection = 1;
	PatrolDelayRemaining = 0.0f;
	PatrolSegmentStartPointIndex = INDEX_NONE;
	bNeedsPatrolSegmentFacingUpdate = true;

	if (!Controller)
	{
		SpawnDefaultController();
	}

	RefreshEnemyCollisionIgnores();
	ApplyEnemyRuntimeSettings();
	InitializeFromArchetype(DefaultArchetype);
}

void APlatformerEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentCombatTarget)
	{
		UpdatePatrolMovement(DeltaTime);
		return;
	}

	if (!CurrentCombatTarget->IsAlive() || GetCombatDistanceToTarget(CurrentCombatTarget) > CombatLoseTargetRange)
	{
		SetCombatTarget(nullptr);
		UpdatePatrolMovement(DeltaTime);
	}
	else
	{
		const float CurrentTargetDistance = GetCombatDistanceToTarget(CurrentCombatTarget);
		if (CurrentTargetDistance <= GetAttackRange())
		{
			ApplyFacingFromDirection(CurrentCombatTarget->GetActorLocation() - GetActorLocation());
			TryAttackCurrentTarget();
		}

		bool bShouldChase = bEnablePlayerChase && (CurrentTargetDistance <= ChaseAgroRadius);
		if (bShouldChase)
		{
			UpdateChaseMovement(DeltaTime);
		}
		else
		{
			UpdatePatrolMovement(DeltaTime);
		}
	}
}

void APlatformerEnemyBase::ApplyEnemyRuntimeSettings()
{
	Health = FMath::Max(Health, 1.0f);
	MovementSpeed = FMath::Max(MovementSpeed, 1.0f);
	Damage = FMath::Max(Damage, 0.0f);
	HitDelay = FMath::Max(HitDelay, 0.0f);
	PatrolDelayTime = FMath::Max(PatrolDelayTime, 0.0f);
	ChaseAgroRadius = FMath::Max(ChaseAgroRadius, 0.0f);
	ProjectileMaxDistance = FMath::Max(ProjectileMaxDistance, 0.0f);

	if (AttributeSet)
	{
		AttributeSet->SetMaxHealth(Health);
		AttributeSet->SetHealth(Health);
		AttributeSet->SetBaseDamage(Damage);
		AttributeSet->SetAttackSpeed(HitDelay > 0.0f ? 1.0f / HitDelay : 0.0f);
		AttributeSet->SetMeleeAttackDamage(Damage);
		AttributeSet->SetRangeBaseAttackDamage(Damage);
		AttributeSet->SetMeleeAttackDelay(HitDelay);
		AttributeSet->SetRangeAttackDelay(HitDelay);
		AttributeSet->SetMoveSpeed(MovementSpeed);
	}

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = MovementSpeed;
		MovementComponent->MaxFlySpeed = MovementSpeed;
	}

	RefreshHealthWidget();
}

void APlatformerEnemyBase::SetEnemyHealth(float InHealth)
{
	Health = FMath::Max(InHealth, 1.0f);
	ApplyEnemyRuntimeSettings();
}

void APlatformerEnemyBase::SetEnemyMovementSpeed(float InMovementSpeed)
{
	MovementSpeed = FMath::Max(InMovementSpeed, 1.0f);
	ApplyEnemyRuntimeSettings();
}

void APlatformerEnemyBase::SetEnemyDamage(float InDamage)
{
	Damage = FMath::Max(InDamage, 0.0f);
	ApplyEnemyRuntimeSettings();
}

void APlatformerEnemyBase::SetEnemyHitDelay(float InHitDelay)
{
	HitDelay = FMath::Max(InHitDelay, 0.0f);
	ApplyEnemyRuntimeSettings();
}

void APlatformerEnemyBase::SetPatrolPoints(const TArray<FVector>& InPatrolPoints)
{
	PatrolPoints = InPatrolPoints;
	CurrentPatrolPointIndex = PatrolPoints.IsValidIndex(CurrentPatrolPointIndex) ? CurrentPatrolPointIndex : 0;
	PatrolDirection = PatrolDirection == 0 ? 1 : PatrolDirection;
	PatrolDelayRemaining = 0.0f;
	PatrolSegmentStartPointIndex = INDEX_NONE;
	bNeedsPatrolSegmentFacingUpdate = true;
}

void APlatformerEnemyBase::SetEnemyProjectileDistance(float InProjectileDistance)
{
	ProjectileMaxDistance = FMath::Max(InProjectileDistance, 0.0f);
}

void APlatformerEnemyBase::SetEnemyPatrolDelayTime(float InPatrolDelayTime)
{
	PatrolDelayTime = FMath::Max(InPatrolDelayTime, 0.0f);
	PatrolDelayRemaining = FMath::Min(PatrolDelayRemaining, PatrolDelayTime);
}

void APlatformerEnemyBase::SetEnablePlayerChase(bool bInEnable)
{
	bEnablePlayerChase = bInEnable;
}

void APlatformerEnemyBase::SetChaseAgroRadius(float InRadius)
{
	ChaseAgroRadius = FMath::Max(InRadius, 0.0f);
}

bool APlatformerEnemyBase::TryAttackCurrentTarget()
{
	return TryAttackTarget(CurrentCombatTarget);
}

bool APlatformerEnemyBase::TryAttackTarget(APlatformerCombatCharacterBase* TargetActor)
{
	if (!CanAttackTarget(TargetActor))
	{
		return false;
	}

	if (!PerformAttack(TargetActor))
	{
		return false;
	}

	if (UWorld* World = GetWorld())
	{
		LastAttackWorldTime = World->GetTimeSeconds();
	}

	return true;
}

void APlatformerEnemyBase::SetCombatTarget(APlatformerCombatCharacterBase* NewTarget)
{
	if (NewTarget == this || (NewTarget && !NewTarget->IsAlive()))
	{
		NewTarget = nullptr;
	}

	if (CurrentCombatTarget == NewTarget)
	{
		return;
	}

	APlatformerCombatCharacterBase* PreviousTarget = CurrentCombatTarget;
	CurrentCombatTarget = NewTarget;
	OnCombatTargetChanged(PreviousTarget, CurrentCombatTarget);
}

void APlatformerEnemyBase::OnCombatDamageReceived(float DamageAmount, const FHitResult& HitResult, AActor* DamageInstigatorActor)
{
	Super::OnCombatDamageReceived(DamageAmount, HitResult, DamageInstigatorActor);

	if (APlatformerCombatCharacterBase* DamageInstigatorCharacter = Cast<APlatformerCombatCharacterBase>(DamageInstigatorActor))
	{
		SetCombatTarget(DamageInstigatorCharacter);
	}

	if (AbilitySystemComponent && DamageAmount >= StaggerThreshold)
	{
		FGameplayEventData EventData;
		EventData.EventMagnitude = DamageAmount;
		EventData.Instigator = DamageInstigatorActor;
		EventData.Target = this;
		AbilitySystemComponent->HandleGameplayEvent(PlatformerGameplayTags::Event_Combat_HitReceived, &EventData);
	}
}

void APlatformerEnemyBase::OnCombatDeath(AActor* DamageInstigatorActor)
{
	Super::OnCombatDeath(DamageInstigatorActor);

	SetCombatTarget(nullptr);

	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic(TEXT("Combat death"));
	}

	if (PerceptionComponent)
	{
		PerceptionComponent->SetComponentTickEnabled(false);
	}

	if (HasAuthority())
	{
		Destroy();
	}
}

float APlatformerEnemyBase::GetDefaultMaxHealth() const
{
	return Health;
}

float APlatformerEnemyBase::GetAttackRange() const
{
	return CombatAttackRange;
}

float APlatformerEnemyBase::GetAttackCooldown() const
{
	if (!AttributeSet)
	{
		return 0.0f;
	}

	const float AttackSpeed = AttributeSet->GetAttackSpeed();
	return AttackSpeed > 0.0f ? 1.0f / AttackSpeed : 0.0f;
}

float APlatformerEnemyBase::GetAttackDamageAmount() const
{
	if (!AttributeSet)
	{
		return 0.0f;
	}

	return FMath::Max(AttributeSet->GetBaseDamage(), 0.0f);
}

float APlatformerEnemyBase::GetProjectileMaxDistance() const
{
	return ProjectileMaxDistance;
}

float APlatformerEnemyBase::GetHealthWidgetVerticalPadding() const
{
	return PlatformerHealthWidgetVerticalPadding;
}

bool APlatformerEnemyBase::CanAttackTarget(const APlatformerCombatCharacterBase* TargetActor) const
{
	if (!TargetActor || !TargetActor->IsAlive() || !IsAlive())
	{
		return false;
	}

	if (GetCombatDistanceToTarget(TargetActor) > GetAttackRange())
	{
		return false;
	}

	if (const UWorld* World = GetWorld())
	{
		const float AttackCooldown = GetAttackCooldown();
		if (AttackCooldown > 0.0f && World->GetTimeSeconds() - LastAttackWorldTime < AttackCooldown)
		{
			return false;
		}
	}

	return true;
}

bool APlatformerEnemyBase::PerformAttack(APlatformerCombatCharacterBase* TargetActor)
{
	return false;
}

void APlatformerEnemyBase::ApplyArchetypeCombatData(const UPlatformerEnemyArchetypeAsset* Archetype)
{
}

void APlatformerEnemyBase::OnCombatTargetChanged(APlatformerCombatCharacterBase* PreviousTarget, APlatformerCombatCharacterBase* NewTarget)
{
	BP_OnCombatTargetChanged(PreviousTarget, NewTarget);
}

float APlatformerEnemyBase::GetCombatDistanceToTarget(const APlatformerCombatCharacterBase* TargetActor) const
{
	if (!TargetActor)
	{
		return TNumericLimits<float>::Max();
	}

	const FVector SourceLocation = GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	const FVector FlattenedSourceLocation(SourceLocation.X, 0.0f, SourceLocation.Z);
	const FVector FlattenedTargetLocation(TargetLocation.X, 0.0f, TargetLocation.Z);
	return FVector::Dist(FlattenedSourceLocation, FlattenedTargetLocation);
}

void APlatformerEnemyBase::RefreshEnemyCollisionIgnores()
{
	UWorld* World = GetWorld();
	if (!World || !GetCapsuleComponent())
	{
		return;
	}

	for (TActorIterator<APlatformerEnemyBase> It(World); It; ++It)
	{
		APlatformerEnemyBase* OtherEnemy = *It;
		if (!OtherEnemy || OtherEnemy == this)
		{
			continue;
		}

		IgnoreCollisionWithEnemy(OtherEnemy);
		OtherEnemy->IgnoreCollisionWithEnemy(this);
	}
}

void APlatformerEnemyBase::IgnoreCollisionWithEnemy(APlatformerEnemyBase* OtherEnemy)
{
	if (!OtherEnemy)
	{
		return;
	}

	if (UCapsuleComponent* EnemyCapsuleComponent = GetCapsuleComponent())
	{
		EnemyCapsuleComponent->IgnoreActorWhenMoving(OtherEnemy, true);
	}
}

void APlatformerEnemyBase::UpdatePatrolMovement(float DeltaTime)
{
	if (!bEnableNativePatrol || PatrolPoints.Num() < 2 || !IsAlive() || DeltaTime <= 0.0f)
	{
		return;
	}

	if (!PatrolPoints.IsValidIndex(CurrentPatrolPointIndex))
	{
		CurrentPatrolPointIndex = 0;
	}

	if (PatrolDirection == 0)
	{
		PatrolDirection = 1;
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	if (MovementComponent->MovementMode == MOVE_None)
	{
		MovementComponent->SetMovementMode(MovementComponent->DefaultLandMovementMode);
	}

	if (PatrolDelayRemaining > 0.0f)
	{
		PatrolDelayRemaining = FMath::Max(0.0f, PatrolDelayRemaining - DeltaTime);
		StopPatrolMovement(MovementComponent);
		if (PatrolDelayRemaining > 0.0f)
		{
			return;
		}
	}

	const float ResolvedAcceptanceRadius = FMath::Max(PatrolAcceptanceRadius, 0.0f);
	FVector MovementDelta = FVector::ZeroVector;
	for (int32 AttemptIndex = 0; AttemptIndex < PatrolPoints.Num(); ++AttemptIndex)
	{
		const FVector ActorLocation = GetActorLocation();
		const FVector TargetLocation = GetPatrolPointWorldLocation(CurrentPatrolPointIndex);
		MovementDelta = TargetLocation - ActorLocation;
		MovementDelta.Y = 0.0f;

		const bool bCanMoveVertically =
			MovementComponent->MovementMode == MOVE_Flying ||
			MovementComponent->DefaultLandMovementMode == MOVE_Flying;
		if (!bCanMoveVertically)
		{
			MovementDelta.Z = 0.0f;
		}

		if (MovementDelta.SizeSquared() > FMath::Square(ResolvedAcceptanceRadius))
		{
			break;
		}

		const int32 ReachedPatrolPointIndex = CurrentPatrolPointIndex;
		AdvancePatrolTargetFromReachedPoint(ReachedPatrolPointIndex);
		if (IsPatrolEndpoint(ReachedPatrolPointIndex) && PatrolDelayTime > 0.0f)
		{
			PatrolDelayRemaining = PatrolDelayTime;
			StopPatrolMovement(MovementComponent);
			return;
		}
	}

	if (MovementDelta.SizeSquared() <= FMath::Square(ResolvedAcceptanceRadius))
	{
		StopPatrolMovement(MovementComponent);
		return;
	}

	const FVector MovementDirection = MovementDelta.GetSafeNormal();
	ApplyFacingForCurrentPatrolSegment();
	
	// Используем AddMovementInput вместо прямого назначения Velocity,
	// чтобы корректно работало ускорение и трение CharacterMovementComponent
	AddMovementInput(MovementDirection, 1.0f);
}

void APlatformerEnemyBase::UpdateChaseMovement(float DeltaTime)
{
	if (!IsAlive() || !CurrentCombatTarget)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	if (MovementComponent->MovementMode == MOVE_None)
	{
		MovementComponent->SetMovementMode(MovementComponent->DefaultLandMovementMode);
	}

	FVector TargetLocation = CurrentCombatTarget->GetActorLocation();
	FVector ActorLocation = GetActorLocation();
	
	FVector MovementDelta = TargetLocation - ActorLocation;
	MovementDelta.Y = 0.0f;

	const bool bCanMoveVertically =
		MovementComponent->MovementMode == MOVE_Flying ||
		MovementComponent->DefaultLandMovementMode == MOVE_Flying;
	if (!bCanMoveVertically)
	{
		MovementDelta.Z = 0.0f;
	}

	// Останавливаемся, если подошли на дистанцию атаки
	const float ResolvedAttackRange = FMath::Max(CombatAttackRange, 50.0f);
	if (MovementDelta.SizeSquared() <= FMath::Square(ResolvedAttackRange * 0.8f))
	{
		StopPatrolMovement(MovementComponent);
		return;
	}

	const FVector MovementDirection = MovementDelta.GetSafeNormal();
	ApplyFacingFromDirection(MovementDirection);
	
	AddMovementInput(MovementDirection, 1.0f);
}

FVector APlatformerEnemyBase::GetPatrolPointWorldLocation(int32 PatrolPointIndex) const
{
	if (!PatrolPoints.IsValidIndex(PatrolPointIndex))
	{
		return PatrolOriginLocation;
	}

	return PatrolOriginLocation + PatrolPoints[PatrolPointIndex];
}

void APlatformerEnemyBase::AdvancePatrolTargetFromReachedPoint(int32 ReachedPatrolPointIndex)
{
	PatrolSegmentStartPointIndex = ReachedPatrolPointIndex;
	bNeedsPatrolSegmentFacingUpdate = true;

	if (PatrolPoints.Num() < 2)
	{
		CurrentPatrolPointIndex = 0;
		PatrolDirection = 1;
		return;
	}

	if (ReachedPatrolPointIndex <= 0)
	{
		PatrolDirection = 1;
		CurrentPatrolPointIndex = 1;
		return;
	}

	const int32 LastPatrolPointIndex = PatrolPoints.Num() - 1;
	if (ReachedPatrolPointIndex >= LastPatrolPointIndex)
	{
		PatrolDirection = -1;
		CurrentPatrolPointIndex = LastPatrolPointIndex - 1;
		return;
	}

	CurrentPatrolPointIndex = FMath::Clamp(
		ReachedPatrolPointIndex + PatrolDirection,
		0,
		LastPatrolPointIndex);
}

bool APlatformerEnemyBase::IsPatrolEndpoint(int32 PatrolPointIndex) const
{
	return PatrolPointIndex == 0 || PatrolPointIndex == PatrolPoints.Num() - 1;
}

void APlatformerEnemyBase::StopPatrolMovement(UCharacterMovementComponent* MovementComponent) const
{
	if (!MovementComponent)
	{
		return;
	}

	MovementComponent->Velocity.X = 0.0f;
	MovementComponent->Velocity.Y = 0.0f;
	if (MovementComponent->MovementMode == MOVE_Flying)
	{
		MovementComponent->Velocity.Z = 0.0f;
	}
}

void APlatformerEnemyBase::ApplyFacingForCurrentPatrolSegment()
{
	if (!bNeedsPatrolSegmentFacingUpdate || !PatrolPoints.IsValidIndex(CurrentPatrolPointIndex))
	{
		return;
	}

	const FVector SegmentStartLocation = PatrolPoints.IsValidIndex(PatrolSegmentStartPointIndex)
		? GetPatrolPointWorldLocation(PatrolSegmentStartPointIndex)
		: GetActorLocation();
	const FVector SegmentTargetLocation = GetPatrolPointWorldLocation(CurrentPatrolPointIndex);

	FVector SegmentDirection = SegmentTargetLocation - SegmentStartLocation;
	SegmentDirection.Y = 0.0f;
	ApplyFacingFromDirection(SegmentDirection);
	bNeedsPatrolSegmentFacingUpdate = false;
}

void APlatformerEnemyBase::ApplyFacingFromDirection(const FVector& MovementDirection)
{
	if (FMath::IsNearlyZero(MovementDirection.X))
	{
		return;
	}

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetRelativeRotation(
			MovementDirection.X >= 0.0f ? EnemyMeshFacingRightRotation : EnemyMeshFacingLeftRotation);
	}
}

void APlatformerEnemyBase::ApplyDefaultMeshFacing()
{
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetRelativeRotation(EnemyMeshDefaultRotation);
	}
}

#if WITH_EDITORONLY_DATA
void APlatformerEnemyBase::RefreshEditorPatrolPreviewComponents()
{
	if (PatrolPathSpline)
	{
		PatrolPathSpline->SetClosedLoop(false);
		PatrolPathSpline->SetSplinePoints(PatrolPoints, ESplineCoordinateSpace::Local, false);

		for (int32 PointIndex = 0; PointIndex < PatrolPoints.Num(); ++PointIndex)
		{
			PatrolPathSpline->SetSplinePointType(PointIndex, ESplinePointType::Linear, false);
		}

		PatrolPathSpline->UpdateSpline();
	}

	while (PatrolPointPreviewMeshes.Num() > PatrolPoints.Num())
	{
		if (UStaticMeshComponent* PreviewMesh = PatrolPointPreviewMeshes.Last())
		{
			PreviewMesh->DestroyComponent();
		}

		PatrolPointPreviewMeshes.RemoveAt(PatrolPointPreviewMeshes.Num() - 1);
	}

	for (int32 PointIndex = 0; PointIndex < PatrolPoints.Num(); ++PointIndex)
	{
		UStaticMeshComponent* PreviewMesh = PatrolPointPreviewMeshes.IsValidIndex(PointIndex)
			? PatrolPointPreviewMeshes[PointIndex]
			: nullptr;

		if (!IsValid(PreviewMesh))
		{
			const FName ComponentName = MakeUniqueObjectName(
				this,
				UStaticMeshComponent::StaticClass(),
				FName(TEXT("PatrolPointPreviewMesh")));

			PreviewMesh = NewObject<UStaticMeshComponent>(this, ComponentName, RF_Transactional | RF_TextExportTransient);
			PreviewMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			PreviewMesh->SetupAttachment(RootComponent);
			PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PreviewMesh->SetHiddenInGame(true);
			PreviewMesh->SetIsVisualizationComponent(true);
			PreviewMesh->bIsEditorOnly = true;
			PreviewMesh->SetMobility(EComponentMobility::Movable);

			if (PatrolPointPreviewMeshAsset)
			{
				PreviewMesh->SetStaticMesh(PatrolPointPreviewMeshAsset);
			}

			PreviewMesh->RegisterComponent();
			if (PatrolPointPreviewMeshes.IsValidIndex(PointIndex))
			{
				PatrolPointPreviewMeshes[PointIndex] = PreviewMesh;
			}
			else
			{
				PatrolPointPreviewMeshes.Add(PreviewMesh);
			}
		}

		PreviewMesh->SetRelativeLocation(PatrolPoints[PointIndex]);
		PreviewMesh->SetRelativeRotation(FRotator::ZeroRotator);
		PreviewMesh->SetRelativeScale3D(FVector(0.25f));
		PreviewMesh->SetVisibility(true);
	}
}
#endif

void APlatformerEnemyBase::HandleTargetPerceptionUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	APlatformerCombatCharacterBase* SensedCombatTarget = Cast<APlatformerCombatCharacterBase>(UpdatedActor);
	if (!SensedCombatTarget || SensedCombatTarget == this)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetCombatDistanceToTarget(SensedCombatTarget) <= CombatEngageRange)
		{
			SetCombatTarget(SensedCombatTarget);
		}
	}
	else if (CurrentCombatTarget == SensedCombatTarget)
	{
		SetCombatTarget(nullptr);
	}
}
