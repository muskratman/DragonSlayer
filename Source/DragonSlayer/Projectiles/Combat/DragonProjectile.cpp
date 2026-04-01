#include "Projectiles/Combat/DragonProjectile.h"
#include "NiagaraComponent.h"

ADragonProjectile::ADragonProjectile()
{
	Speed = 2000.0f;
	Lifetime = 3.0f;
	bPiercing = false;

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailVFX"));
	TrailVFX->SetupAttachment(RootComponent);
}
