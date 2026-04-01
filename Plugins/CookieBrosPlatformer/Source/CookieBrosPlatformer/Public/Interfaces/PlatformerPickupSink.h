#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlatformerPickupSink.generated.h"

class ACharacter;
class APlatformerPickup;

UINTERFACE(MinimalAPI)
class UPlatformerPickupSink : public UInterface
{
	GENERATED_BODY()
};

/**
 * IPlatformerPickupSink
 * Service contract for runtime owners that process collected pickups.
 */
class COOKIEBROSPLATFORMER_API IPlatformerPickupSink
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Pickup")
	void ProcessPlatformerPickup(APlatformerPickup* Pickup, ACharacter* Collector);
};
