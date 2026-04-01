#pragma once

#include "CoreMinimal.h"
#include "Platformer/Environment/PlatformerPointToPointMover.h"
#include "PlatformerPatrolPlatform.generated.h"

/**
 * Native ping-pong moving platform with configurable pauses at both endpoints.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerPatrolPlatform : public APlatformerPointToPointMover
{
	GENERATED_BODY()

public:
	APlatformerPatrolPlatform();

protected:
	virtual void BeginPlay() override;
	virtual void HandlePauseFinishedAtPointA() override;
	virtual void HandlePauseFinishedAtPointB() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol Platform")
	bool bAutoStart = true;
};
