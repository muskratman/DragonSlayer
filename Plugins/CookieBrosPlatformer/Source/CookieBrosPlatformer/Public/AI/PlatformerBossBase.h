#pragma once

#include "CoreMinimal.h"
#include "AI/PlatformerEnemyBase.h"
#include "PlatformerBossBase.generated.h"

/**
 * APlatformerBossBase
 * Generic multi-phase platformer boss shell with encounter activation hooks.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerBossBase : public APlatformerEnemyBase
{
	GENERATED_BODY()

public:
	APlatformerBossBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Boss")
	FText BossEncounterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Boss", meta=(ClampMin=1, UIMin=1))
	int32 TotalPhases = 1;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Boss")
	void TransitionToPhase(int32 NextPhase);

protected:
	virtual void BeginPlay() override;

	int32 CurrentPhase = 1;
};
