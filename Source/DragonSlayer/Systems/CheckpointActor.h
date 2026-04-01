#pragma once

#include "CoreMinimal.h"
#include "Platformer/Systems/PlatformerCheckpointActor.h"
#include "CheckpointActor.generated.h"

class UNiagaraComponent;

/**
 * ACheckpointActor
 * Shrine that the player interacts with to save progress and respawn.
 */
UCLASS()
class DRAGONSLAYER_API ACheckpointActor : public APlatformerCheckpointActor
{
	GENERATED_BODY()
	
public:	
	ACheckpointActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> ShrineVFX;
};
