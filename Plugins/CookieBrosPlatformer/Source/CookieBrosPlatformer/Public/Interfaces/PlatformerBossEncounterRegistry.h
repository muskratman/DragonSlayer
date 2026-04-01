#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlatformerBossEncounterRegistry.generated.h"

class APlatformerBossBase;

UINTERFACE(MinimalAPI)
class UPlatformerBossEncounterRegistry : public UInterface
{
	GENERATED_BODY()
};

/**
 * IPlatformerBossEncounterRegistry
 * Service contract for runtime owners that react to boss encounter activation.
 */
class COOKIEBROSPLATFORMER_API IPlatformerBossEncounterRegistry
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Boss")
	void RegisterBossEncounterActor(APlatformerBossBase* Boss);
};
