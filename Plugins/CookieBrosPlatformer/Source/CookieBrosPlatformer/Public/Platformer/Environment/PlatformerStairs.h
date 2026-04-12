#pragma once

#include "CoreMinimal.h"
#include "Platformer/Environment/PlatformerBlockBase.h"
#include "PlatformerStairs.generated.h"

/**
 * One-tile 45-degree stair step with block-like sizing and collision ownership.
 * Uses the standard block import path but remains a distinct class for separate visuals.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerStairs : public APlatformerBlockBase
{
	GENERATED_BODY()

public:
	APlatformerStairs();
};
