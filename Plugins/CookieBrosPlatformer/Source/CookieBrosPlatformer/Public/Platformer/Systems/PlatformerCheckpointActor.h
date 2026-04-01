#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Interfaces/Interactable.h"
#include "PlatformerCheckpointActor.generated.h"

class UBoxComponent;

/**
 * APlatformerCheckpointActor
 * Generic checkpoint actor that registers itself through service interfaces.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerCheckpointActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	APlatformerCheckpointActor();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Checkpoint")
	FGameplayTag CheckpointID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Checkpoint")
	FText CheckpointName;

	FORCEINLINE bool IsCheckpointActive() const { return bIsActive; }
	FORCEINLINE UBoxComponent* GetTriggerZone() const { return TriggerZone; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> TriggerZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Checkpoint")
	bool bIsActive = false;
};
