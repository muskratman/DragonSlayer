#pragma once

#include "CoreMinimal.h"

class AActor;
class UGameplayEffect;
struct FHitResult;

namespace PlatformerEnvironment
{
	bool ApplyConfiguredDamage(AActor* SourceActor, AActor* TargetActor, TSubclassOf<UGameplayEffect> DamageEffectClass, float DirectDamage, const FHitResult& HitResult);
}
