#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PlatformerAnimDataAsset.generated.h"

class UAnimMontage;

/**
 * Single entry mapping a GameplayTag to an AnimMontage.
 */
USTRUCT(BlueprintType)
struct FPlatformerAbilityAnimEntry
{
	GENERATED_BODY()

	/** GameplayTag identifying the ability/action (e.g., "Anim.Combat.MeleeHit") */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation",
	          meta=(Categories="Anim"))
	FGameplayTag AnimTag;

	/** AnimMontage to play for this ability */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> Montage;

	/** Default play rate for this montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation",
	          meta=(ClampMin="0.01"))
	float DefaultPlayRate = 1.0f;
};

/**
 * UPlatformerAnimDataAsset
 * Data-driven mapping of GameplayTag → AnimMontage for ability animations.
 * Projects create instances of this asset and assign them to characters.
 */
UCLASS(BlueprintType)
class COOKIEBROSPLATFORMER_API UPlatformerAnimDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability Animations")
	TArray<FPlatformerAbilityAnimEntry> AbilityAnimations;

	/** Find the full animation entry for a given tag. Returns nullptr if not found. */
	const FPlatformerAbilityAnimEntry* FindAnimEntry(const FGameplayTag& AnimTag) const;

	/** Shortcut: returns the montage for a given tag, or nullptr. */
	UAnimMontage* FindMontage(const FGameplayTag& AnimTag) const;
};
