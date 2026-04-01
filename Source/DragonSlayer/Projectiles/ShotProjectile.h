#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Combat/DragonProjectile.h"
#include "ShotProjectile.generated.h"

/**
 * AShotProjectile
 * Concrete child class used for BaseShot and ChargeShot.
 */
UCLASS()
class DRAGONSLAYER_API AShotProjectile : public ADragonProjectile
{
	GENERATED_BODY()
	
public:
	AShotProjectile();
};
