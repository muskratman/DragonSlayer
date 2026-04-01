#include "Projectiles/Combat/CombatProjectile.h"
#include "Interfaces/Damageable.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACombatProjectile::ACombatProjectile()
{
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void ACombatProjectile::HandleImpact(AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, const FHitResult& Hit)
{
	ApplyCombatEffects(OtherActor, Hit);

	if (!bPiercing)
	{
		Destroy();
	}
}

void ACombatProjectile::ApplyCombatEffects(AActor* Target, const FHitResult& Hit)
{
	if (IDamageable* DamageableTarget = Cast<IDamageable>(Target))
	{
		if (DamageEffectSpec.IsValid())
		{
			DamageableTarget->ApplyDamage(DamageEffectSpec, Hit);
		}

		if (StatusEffectSpec.IsValid())
		{
			DamageableTarget->ApplyDamage(StatusEffectSpec, Hit);
		}
	}
}
