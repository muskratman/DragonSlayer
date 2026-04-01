#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlatformerLadder.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class ACharacter;
class USceneComponent;
class UStaticMeshComponent;
class UTexture2D;

USTRUCT()
struct FPlatformerLadderState
{
	GENERATED_BODY()

	UPROPERTY()
	float GravityScale = 1.0f;

	UPROPERTY()
	TEnumAsByte<EMovementMode> MovementMode = MOVE_Walking;
};

/**
 * Ladder volume that temporarily places characters into climb-friendly movement.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerLadder : public AActor
{
	GENERATED_BODY()

public:
	APlatformerLadder();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> LadderMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> ClimbVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Shape")
	FVector LadderSize = FVector(40.0f, 120.0f, 400.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	float ClimbGravityScale = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	bool bUseFlyingMovementMode = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ladder|Climb")
	bool bSnapCharacterDepthToLadder = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

	TMap<TWeakObjectPtr<ACharacter>, FPlatformerLadderState> ClimbingCharacters;

	UFUNCTION()
	void OnClimbVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnClimbVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void RestoreCharacter(ACharacter* Character);
};
