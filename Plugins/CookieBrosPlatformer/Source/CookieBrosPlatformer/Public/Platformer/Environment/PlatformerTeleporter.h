#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerTeleporter.generated.h"

class UArrowComponent;
class UBoxComponent;
class UPrimitiveComponent;
class UTexture2D;

/**
 * Entrance/exit teleporter actor pair for rapid traversal and prototype routing.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerTeleporter : public AActor
{
	GENERATED_BODY()

public:
	APlatformerTeleporter();

	void RegisterArrival(AActor* Actor);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> ExitPoint;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Teleporter")
	TObjectPtr<APlatformerTeleporter> DestinationTeleporter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teleporter")
	FVector TriggerExtent = FVector(100.0f, 100.0f, 150.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teleporter", meta=(ClampMin=0.0, Units="s"))
	float TeleportCooldown = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teleporter")
	bool bKeepVelocity = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teleporter", meta=(ClampMin=0.0, Units="cm/s"))
	float ExitSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Editor|Palette")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

	TMap<TWeakObjectPtr<AActor>, float> RecentlyTeleportedActors;

	UFUNCTION()
	void OnTriggerVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool CanTeleportActor(AActor* Actor) const;
	void MarkActorTeleported(AActor* Actor);
	FVector GetExitLocation() const;
	FRotator GetExitRotation() const;
};
