#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlatformerGravityVolume.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class ACharacter;
class UTexture2D;

USTRUCT()
struct FPlatformerGravityVolumeState
{
	GENERATED_BODY()

	UPROPERTY()
	float GravityScale = 1.0f;

	UPROPERTY()
	TEnumAsByte<EMovementMode> MovementMode = MOVE_Walking;
};

/**
 * Volume that overrides character gravity for water-like or low/high gravity sections.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerGravityVolume : public AActor
{
	GENERATED_BODY()

public:
	APlatformerGravityVolume();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity Volume")
	FVector VolumeExtent = FVector(200.0f, 200.0f, 200.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity Volume")
	float GravityScaleOverride = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity Volume")
	bool bUseFlyingMovementMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity Volume", meta=(ClampMin=0.0, Units="cm/s"))
	float MaxGravityVelocity = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

	TMap<TWeakObjectPtr<ACharacter>, FPlatformerGravityVolumeState> AffectedCharacters;

	UFUNCTION()
	void OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void RestoreCharacter(ACharacter* Character);
};
