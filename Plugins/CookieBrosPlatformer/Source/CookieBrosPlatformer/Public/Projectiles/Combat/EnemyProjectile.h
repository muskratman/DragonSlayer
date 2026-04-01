#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Combat/CombatProjectile.h"
#include "EnemyProjectile.generated.h"

/**
 * AEnemyProjectile
 * Base class for all hostile projectiles.
 */
UCLASS(abstract)
class COOKIEBROSPLATFORMER_API AEnemyProjectile : public ACombatProjectile
{
	GENERATED_BODY()

public:
	AEnemyProjectile();
};
