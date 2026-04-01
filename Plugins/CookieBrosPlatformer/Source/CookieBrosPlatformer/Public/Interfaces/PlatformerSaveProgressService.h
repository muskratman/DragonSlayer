#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlatformerSaveProgressService.generated.h"

UINTERFACE(MinimalAPI)
class UPlatformerSaveProgressService : public UInterface
{
	GENERATED_BODY()
};

/**
 * IPlatformerSaveProgressService
 * Service contract for saving platformer progression when generic runtime actors request it.
 */
class COOKIEBROSPLATFORMER_API IPlatformerSaveProgressService
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Save")
	bool SavePlatformerProgress();
};
