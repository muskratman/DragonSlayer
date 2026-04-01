#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PlatformerEnemyArchetypeAsset.generated.h"

class APlatformerEnemyBase;
class UGameplayAbility;
class UStateTree;

/**
 * UPlatformerEnemyArchetypeAsset
 * Generic runtime configuration for reusable platformer enemies.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API UPlatformerEnemyArchetypeAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Identity")
	FGameplayTag EnemyTypeTag;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<APlatformerEnemyBase> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	float BaseHealth = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	float BaseDamage = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	float AttackSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	float MoveSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UStateTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
};
