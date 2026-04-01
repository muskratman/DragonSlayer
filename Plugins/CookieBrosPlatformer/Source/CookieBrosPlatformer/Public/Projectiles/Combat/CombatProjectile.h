#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Projectiles/BaseProjectile.h"
#include "CombatProjectile.generated.h"

class UPrimitiveComponent;

/**
 * ACombatProjectile
 * Shared GAS-aware projectile layer for player and enemy combat shots.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API ACombatProjectile : public ABaseProjectile
{
	GENERATED_BODY()

public:
	ACombatProjectile();

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn=true), Category="Damage")
	FGameplayEffectSpecHandle DamageEffectSpec;

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn=true), Category="Damage")
	FGameplayEffectSpecHandle StatusEffectSpec;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Projectile")
	bool bPiercing = false;

protected:
	virtual void HandleImpact(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& Hit) override;
	virtual void ApplyCombatEffects(AActor* Target, const FHitResult& Hit);
};
