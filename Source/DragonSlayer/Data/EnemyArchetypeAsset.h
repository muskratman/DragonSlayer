#pragma once

#include "CoreMinimal.h"
#include "Data/PlatformerEnemyArchetypeAsset.h"
#include "GameplayTagContainer.h"
#include "EnemyArchetypeAsset.generated.h"

/**
 * UEnemyArchetypeAsset
 * Data asset containing stats, behavior, and abilities for an enemy type.
 */
UCLASS()
class DRAGONSLAYER_API UEnemyArchetypeAsset : public UPlatformerEnemyArchetypeAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Rewards")
	int32 CurrencyDrop = 10;
    
	UPROPERTY(EditDefaultsOnly, Category="Rewards")
	float OverdriveEnergyDrop = 5.0f;
    
	UPROPERTY(EditDefaultsOnly, Category="Modifiers")
	TArray<FGameplayTag> Immunities;
};
