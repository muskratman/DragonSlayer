#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Platformer/Environment/PlatformerComponentTransformOverride.h"
#include "PlatformerLadder.generated.h"

class UBoxComponent;
class UHierarchicalInstancedStaticMeshComponent;
class UPrimitiveComponent;
class USceneComponent;
class UTexture2D;
class APlatformerCharacterBase;

/**
 * Ladder volume that temporarily places characters into climb-friendly movement.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerLadder : public AActor
{
	GENERATED_BODY()

public:
	APlatformerLadder();
	void SetLadderSize(const FVector& InLadderSize);
	void SetClimbVolumeTransformOffset(const FPlatformerComponentTransformOffset& InTransformOffset);
	void SetSnapCharacterDepthToLadder(bool bInSnapCharacterDepthToLadder);

	FORCEINLINE const FVector& GetLadderSize() const { return LadderSize; }
	FORCEINLINE float GetClimbGravityScale() const { return ClimbGravityScale; }
	FORCEINLINE float GetClimbSpeed() const { return ClimbSpeed; }
	FORCEINLINE float GetJumpClimbSpeed() const { return JumpClimbSpeed; }
	FORCEINLINE float GetJumpClimbDuration() const { return JumpClimbDuration; }
	FORCEINLINE bool UsesFlyingMovementMode() const { return bUseFlyingMovementMode; }
	FORCEINLINE bool ShouldSnapCharacterHorizontalToLadder() const { return bSnapCharacterHorizontalToLadder; }
	FORCEINLINE bool ShouldSnapCharacterDepthToLadder() const { return bSnapCharacterDepthToLadder; }
	FORCEINLINE bool ShouldSnapCharacterRotationToLadder() const { return bSnapCharacterRotationToLadder; }
	FORCEINLINE FRotator GetCharacterLadderRotation() const { return CharacterLadderRotation; }
	float GetClimbBottomWorldZ() const;
	float GetClimbTopWorldZ() const;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> LadderMeshLayoutRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> LadderMeshInstances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> ClimbVolumeLayoutRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> ClimbVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Shape")
	FVector LadderSize = FVector(100.0f, 120.0f, 400.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Components")
	FPlatformerComponentTransformOffset LadderMeshTransformOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Components")
	FPlatformerComponentTransformOffset ClimbVolumeTransformOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	float ClimbGravityScale = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb", meta=(ClampMin="0.0", Units="cm/s"))
	float ClimbSpeed = 320.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb", meta=(ClampMin="0.0", Units="cm/s"))
	float JumpClimbSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb", meta=(ClampMin="0.0", Units="s"))
	float JumpClimbDuration = 0.16f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	bool bUseFlyingMovementMode = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	bool bSnapCharacterHorizontalToLadder = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	bool bSnapCharacterDepthToLadder = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	bool bSnapCharacterRotationToLadder = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb", meta=(EditCondition="bSnapCharacterRotationToLadder"))
	FRotator CharacterLadderRotation = FRotator(0.0f, -90.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

	UFUNCTION()
	void OnClimbVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnClimbVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void RefreshLadderLayout();
	void SyncOverlappingCharacters();
};
