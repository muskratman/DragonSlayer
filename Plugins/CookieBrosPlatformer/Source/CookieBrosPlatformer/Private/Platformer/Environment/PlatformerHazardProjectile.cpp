#include "Platformer/Environment/PlatformerHazardProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Platformer/Environment/PlatformerEnvironmentHelpers.h"

APlatformerHazardProjectile::APlatformerHazardProjectile()
{
	bPiercing = false;
}

void APlatformerHazardProjectile::InitializeProjectile(float InSpeed, float InLifetime, float InDamageAmount, TSubclassOf<UGameplayEffect> InDamageEffectClass, AActor* InDamageSourceActor)
{
	Speed = InSpeed;
	Lifetime = InLifetime;
	DamageAmount = InDamageAmount;
	DamageEffectClass = InDamageEffectClass;
	DamageSourceActor = InDamageSourceActor;

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = InSpeed;
		ProjectileMovement->MaxSpeed = InSpeed;
		ProjectileMovement->Velocity = GetActorForwardVector() * InSpeed;
	}
}

void APlatformerHazardProjectile::HandleImpact(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor == DamageSourceActor.Get())
	{
		return;
	}

	PlatformerEnvironment::ApplyConfiguredDamage(DamageSourceActor.IsValid() ? DamageSourceActor.Get() : this, OtherActor, DamageEffectClass, DamageAmount, Hit);

	if (!bPiercing)
	{
		Destroy();
	}
}
