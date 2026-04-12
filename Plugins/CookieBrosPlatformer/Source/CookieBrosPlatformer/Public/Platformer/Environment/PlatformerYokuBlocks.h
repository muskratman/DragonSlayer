#pragma once

#include "CoreMinimal.h"
#include "Platformer/Environment/PlatformerBlockBase.h"
#include "PlatformerYokuBlocks.generated.h"

/**
 * Cyclic appearing/disappearing platform block used for timing-based traversal sections.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerYokuBlocks : public APlatformerBlockBase
{
	GENERATED_BODY()

public:
	APlatformerYokuBlocks();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Yoku Block|Behaviour", meta=(ClampMin=0.0, Units="s"))
	float InitialDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Yoku Block|Behaviour", meta=(ClampMin=0.0, Units="s"))
	float ShowDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Yoku Block|Behaviour", meta=(ClampMin=0.0, Units="s"))
	float HidenDuration = 1.5f;

	UPROPERTY(Transient)
	bool bIsBlockVisible = true;

	FTimerHandle StateTimerHandle;

	UFUNCTION()
	void ShowBlock();

	UFUNCTION()
	void HideBlock();

	void SetBlockVisibleState(bool bNewVisible);
	void ScheduleNextStateTransition(float Delay, void (APlatformerYokuBlocks::*NextStateFunction)());
};
