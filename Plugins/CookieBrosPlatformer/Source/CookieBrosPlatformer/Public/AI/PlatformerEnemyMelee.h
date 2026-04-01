#pragma once

#include "CoreMinimal.h"
#include "AI/PlatformerEnemyBase.h"
#include "PlatformerEnemyMelee.generated.h"

/**
 * APlatformerEnemyMelee
 * Generic close-range enemy shell.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerEnemyMelee : public APlatformerEnemyBase
{
	GENERATED_BODY()

public:
	APlatformerEnemyMelee();
};
