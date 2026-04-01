#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Combat/EnemyProjectile.h"
#include "PlatformerHazardProjectile.generated.h"

class UGameplayEffect;
class UPrimitiveComponent;

/**
 * Prototype-friendly projectile used by environment hazards and turrets.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerHazardProjectile : public AEnemyProjectile
{
	GENERATED_BODY()

public:
	APlatformerHazardProjectile();

	void InitializeProjectile(float InSpeed, float InLifetime, float InDamageAmount, TSubclassOf<UGameplayEffect> InDamageEffectClass, AActor* InDamageSourceActor);

protected:
	virtual void HandleImpact(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile|Damage", meta=(ClampMin=0.0))
	float DamageAmount = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY()
	TWeakObjectPtr<AActor> DamageSourceActor;
};
