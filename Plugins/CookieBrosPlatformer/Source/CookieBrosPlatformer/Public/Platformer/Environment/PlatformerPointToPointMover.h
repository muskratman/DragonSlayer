#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerPointToPointMover.generated.h"

class UArrowComponent;
class USceneComponent;
class UStaticMeshComponent;
class UTexture2D;

UENUM(BlueprintType)
enum class EPlatformerMoverState : uint8
{
	IdleAtPointA,
	MovingToPointB,
	IdleAtPointB,
	MovingToPointA
};

/**
 * Shared native movement base for point-to-point platforming actors.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerPointToPointMover : public AActor
{
	GENERATED_BODY()

public:
	APlatformerPointToPointMover();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> PointA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> PointB;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mover|Shape")
	FVector PlatformSize = FVector(250.0f, 250.0f, 40.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mover|Movement", meta=(ClampMin=1.0, Units="cm/s"))
	float MoveSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mover|Movement", meta=(ClampMin=0.0, Units="s"))
	float PauseTimeAtPointA = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mover|Movement", meta=(ClampMin=0.0, Units="s"))
	float PauseTimeAtPointB = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mover|Movement", meta=(ClampMin=0.0, Units="cm"))
	float ArrivalTolerance = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mover|Movement")
	bool bStartAtPointB = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

	FVector CachedPointALocation = FVector::ZeroVector;
	FVector CachedPointBLocation = FVector::ZeroVector;
	EPlatformerMoverState MoveState = EPlatformerMoverState::IdleAtPointA;
	float PauseTimer = 0.0f;

	void MoveTowards(const FVector& TargetLocation, float DeltaTime, EPlatformerMoverState ArrivalState);
	void EnterPauseState(EPlatformerMoverState NewState);
	void AdvancePause(float DeltaTime);

	virtual void HandleReachedPointA();
	virtual void HandleReachedPointB();
	virtual void HandlePauseFinishedAtPointA();
	virtual void HandlePauseFinishedAtPointB();

	void StartMovingToPointA();
	void StartMovingToPointB();

public:
	FORCEINLINE UStaticMeshComponent* GetPlatformMesh() const { return PlatformMesh; }
	FORCEINLINE bool IsAtPointA() const { return MoveState == EPlatformerMoverState::IdleAtPointA; }
	FORCEINLINE bool IsAtPointB() const { return MoveState == EPlatformerMoverState::IdleAtPointB; }
	FORCEINLINE bool IsMoving() const
	{
		return MoveState == EPlatformerMoverState::MovingToPointA || MoveState == EPlatformerMoverState::MovingToPointB;
	}
};
