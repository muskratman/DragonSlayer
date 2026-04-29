#pragma once

#include "CoreMinimal.h"
#include "Combat/PlatformerCombatCharacterBase.h"
#include "Perception/AIPerceptionTypes.h"
#include "PlatformerEnemyBase.generated.h"

class UAIPerceptionComponent;
class UPlatformerEnemyArchetypeAsset;
class UStateTreeComponent;
class UGameplayEffect;
class UAISenseConfig_Damage;
class UAISenseConfig_Sight;
class AEnemyProjectile;
class USplineComponent;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * APlatformerEnemyBase
 * Generic GAS-aware enemy shell with perception and StateTree hooks.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerEnemyBase : public APlatformerCombatCharacterBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAISenseConfig_Damage> DamageSenseConfig;

	UPROPERTY(EditDefaultsOnly, Category="Archetype")
	TObjectPtr<UPlatformerEnemyArchetypeAsset> DefaultArchetype;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Targeting", meta=(ClampMin=0.0, Units="cm"))
	float CombatEngageRange = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Targeting", meta=(ClampMin=0.0, Units="cm"))
	float CombatLoseTargetRange = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Attack", meta=(ClampMin=0.0, Units="cm"))
	float CombatAttackRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Attack")
	TSubclassOf<UGameplayEffect> AttackDamageEffectClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Combat|Targeting", meta=(AllowPrivateAccess="true"))
	TObjectPtr<APlatformerCombatCharacterBase> CurrentCombatTarget;

	float StaggerThreshold = 2.0f;
	float LastAttackWorldTime = -BIG_NUMBER;

public:
	APlatformerEnemyBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeFromArchetype(const UPlatformerEnemyArchetypeAsset* Archetype);
	UFUNCTION(BlueprintCallable, Category="AI|Settings")
	virtual void ApplyEnemyRuntimeSettings();

	virtual bool TryAttackCurrentTarget();
	virtual bool TryAttackTarget(APlatformerCombatCharacterBase* TargetActor);
	virtual void SetCombatTarget(APlatformerCombatCharacterBase* NewTarget);

	UFUNCTION(BlueprintPure, Category="AI|Settings")
	float GetEnemyHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category="AI|Settings")
	void SetEnemyHealth(float InHealth);

	UFUNCTION(BlueprintPure, Category="AI|Settings")
	float GetEnemyMovementSpeed() const { return MovementSpeed; }

	UFUNCTION(BlueprintCallable, Category="AI|Settings")
	void SetEnemyMovementSpeed(float InMovementSpeed);

	UFUNCTION(BlueprintPure, Category="AI|Settings")
	float GetEnemyDamage() const { return Damage; }

	UFUNCTION(BlueprintCallable, Category="AI|Settings")
	void SetEnemyDamage(float InDamage);

	UFUNCTION(BlueprintPure, Category="AI|Settings")
	float GetEnemyHitDelay() const { return HitDelay; }

	UFUNCTION(BlueprintCallable, Category="AI|Settings")
	void SetEnemyHitDelay(float InHitDelay);

	UFUNCTION(BlueprintPure, Category="AI|Patrol")
	TArray<FVector> GetPatrolPoints() const { return PatrolPoints; }

	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	void SetPatrolPoints(const TArray<FVector>& InPatrolPoints);

	UFUNCTION(BlueprintPure, Category="AI|Patrol")
	float GetEnemyPatrolDelayTime() const { return PatrolDelayTime; }

	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	void SetEnemyPatrolDelayTime(float InPatrolDelayTime);

	UFUNCTION(BlueprintPure, Category="AI|Combat")
	bool GetEnablePlayerChase() const { return bEnablePlayerChase; }

	UFUNCTION(BlueprintCallable, Category="AI|Combat")
	void SetEnablePlayerChase(bool bInEnable);

	UFUNCTION(BlueprintPure, Category="AI|Combat")
	float GetChaseAgroRadius() const { return ChaseAgroRadius; }

	UFUNCTION(BlueprintCallable, Category="AI|Combat")
	void SetChaseAgroRadius(float InRadius);

	UFUNCTION(BlueprintPure, Category="Combat|Projectile")
	float GetEnemyProjectileDistance() const { return GetProjectileMaxDistance(); }

	UFUNCTION(BlueprintCallable, Category="Combat|Projectile")
	void SetEnemyProjectileDistance(float InProjectileDistance);

	FORCEINLINE UStateTreeComponent* GetStateTreeComponent() const { return StateTreeComponent; }
	FORCEINLINE UAIPerceptionComponent* GetPerceptionComponent() const { return PerceptionComponent; }
	FORCEINLINE APlatformerCombatCharacterBase* GetCombatTarget() const { return CurrentCombatTarget; }
	FORCEINLINE bool HasCombatTarget() const { return CurrentCombatTarget && CurrentCombatTarget->IsAlive(); }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnCombatDamageReceived(float DamageAmount, const FHitResult& HitResult, AActor* DamageInstigatorActor) override;
	virtual void OnCombatDeath(AActor* DamageInstigatorActor) override;
	virtual float GetDefaultMaxHealth() const;
	virtual float GetAttackRange() const;
	virtual float GetAttackCooldown() const;
	virtual float GetAttackDamageAmount() const;
	virtual float GetProjectileMaxDistance() const;
	virtual float GetHealthWidgetVerticalPadding() const override;
	virtual bool CanAttackTarget(const APlatformerCombatCharacterBase* TargetActor) const;
	virtual bool PerformAttack(APlatformerCombatCharacterBase* TargetActor);
	virtual void ApplyArchetypeCombatData(const UPlatformerEnemyArchetypeAsset* Archetype);
	virtual void OnCombatTargetChanged(APlatformerCombatCharacterBase* PreviousTarget, APlatformerCombatCharacterBase* NewTarget);
	float GetCombatDistanceToTarget(const APlatformerCombatCharacterBase* TargetActor) const;
	void RefreshEnemyCollisionIgnores();
	void IgnoreCollisionWithEnemy(APlatformerEnemyBase* OtherEnemy);
	void UpdatePatrolMovement(float DeltaTime);
	void UpdateChaseMovement(float DeltaTime);
	FVector GetPatrolPointWorldLocation(int32 PatrolPointIndex) const;
	void AdvancePatrolTargetFromReachedPoint(int32 ReachedPatrolPointIndex);
	bool IsPatrolEndpoint(int32 PatrolPointIndex) const;
	void StopPatrolMovement(UCharacterMovementComponent* MovementComponent) const;
	void ApplyFacingForCurrentPatrolSegment();
	void ApplyFacingFromDirection(const FVector& MovementDirection);
	void ApplyDefaultMeshFacing();
#if WITH_EDITORONLY_DATA
	void RefreshEditorPatrolPreviewComponents();
#endif

	UFUNCTION(BlueprintImplementableEvent, Category="Combat", meta=(DisplayName="On Combat Target Changed"))
	void BP_OnCombatTargetChanged(AActor* PreviousTarget, AActor* NewTarget);

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* UpdatedActor, FAIStimulus Stimulus);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true", ClampMin=0.0, Units="cm"))
	float PlatformerHealthWidgetVerticalPadding = 20.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Settings", meta=(ClampMin=1.0))
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Settings", meta=(ClampMin=1.0, Units="cm/s"))
	float MovementSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Settings", meta=(ClampMin=0.0))
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Settings", meta=(ClampMin=0.0, Units="s"))
	float HitDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Patrol", meta=(DisplayName="Patrol Points (Relative)", MakeEditWidget=true))
	TArray<FVector> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Patrol")
	bool bEnableNativePatrol = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Combat")
	bool bEnablePlayerChase = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Combat", meta=(ClampMin=0.0, Units="cm"))
	float ChaseAgroRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|Patrol", meta=(ClampMin=0.0, Units="cm"))
	float PatrolAcceptanceRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Patrol", meta=(ClampMin=0.0, Units="s"))
	float PatrolDelayTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Projectile", meta=(ClampMin=0.0, Units="cm"))
	float ProjectileMaxDistance = 600.0f;

	UPROPERTY(Transient)
	FVector PatrolOriginLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	int32 CurrentPatrolPointIndex = 0;

	UPROPERTY(Transient)
	int32 PatrolDirection = 1;

	UPROPERTY(Transient)
	float PatrolDelayRemaining = 0.0f;

	UPROPERTY(Transient)
	int32 PatrolSegmentStartPointIndex = INDEX_NONE;

	UPROPERTY(Transient)
	bool bNeedsPatrolSegmentFacingUpdate = true;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Editor")
	TObjectPtr<USplineComponent> PatrolPathSpline;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> PatrolPointPreviewMeshAsset;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> PatrolPointPreviewMeshes;
#endif
};
