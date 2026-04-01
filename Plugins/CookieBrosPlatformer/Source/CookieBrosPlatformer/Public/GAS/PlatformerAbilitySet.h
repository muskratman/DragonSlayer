#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "PlatformerAbilitySet.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FPlatformerAbilitySet_Ability
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

USTRUCT(BlueprintType)
struct FPlatformerAbilitySet_Effect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * UPlatformerAbilitySet
 * Generic DataAsset that holds abilities and startup effects granted to a platformer character.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API UPlatformerAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TArray<FPlatformerAbilitySet_Ability> Abilities;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TArray<FPlatformerAbilitySet_Effect> Effects;
};
