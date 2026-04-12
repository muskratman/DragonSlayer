#pragma once

#include "CoreMinimal.h"
#include "Platformer/Environment/PlatformerBlockBase.h"
#include "PlatformerLedgeGrab.generated.h"

class ACharacter;
class UBoxComponent;
class UPrimitiveComponent;
class USceneComponent;
struct FPlatformerLedgeTraversalSettings;

/**
 * Specialized hangable ledge block used as an explicit traversal anchor.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerLedgeGrab : public APlatformerBlockBase
{
	GENERATED_BODY()

public:
	APlatformerLedgeGrab();

	bool TryBuildTraversalTargets(
		const ACharacter& Character,
		float DirectionSign,
		float TargetDepthY,
		const FPlatformerLedgeTraversalSettings& LedgeSettings,
		FVector& OutHangLocation,
		FVector& OutClimbTargetLocation,
		FVector& OutWallNormal) const;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> LedgeTriggerLayoutRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> LedgeTriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ledge Grab|Trigger", meta=(ClampMin=0.0, Units="cm"))
	float TriggerHorizontalPadding = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ledge Grab|Trigger", meta=(ClampMin=0.0, Units="cm"))
	float TriggerVerticalExtent = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ledge Grab|Components")
	FPlatformerComponentTransformOffset LedgeTriggerTransformOffset;

	UFUNCTION()
	void OnLedgeTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnLedgeTriggerEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
