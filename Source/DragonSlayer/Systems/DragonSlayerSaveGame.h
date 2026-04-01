#pragma once

#include "CoreMinimal.h"
#include "Core/SaveGame/PlatformerSaveGame.h"
#include "GameplayTagContainer.h"
#include "DragonSlayerSaveGame.generated.h"

UENUM(BlueprintType)
enum class ELevelCompletionStatus : uint8
{
	Locked,
	Unlocked,
	Completed,
	Perfected
};

USTRUCT(BlueprintType)
struct FPlayerProgressionData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TMap<FGameplayTag, bool> UnlockedRegions;

	UPROPERTY(SaveGame)
	TMap<FGameplayTag, ELevelCompletionStatus> LevelStates;

	UPROPERTY(SaveGame)
	TSet<FGameplayTag> UnlockedForms;

	UPROPERTY(SaveGame)
	TSet<FGameplayTag> PurchasedUpgrades;

	UPROPERTY(SaveGame)
	TSet<FName> CollectedSecrets;

	UPROPERTY(SaveGame)
	int32 UpgradeCurrency = 0;

	UPROPERTY(SaveGame)
	int32 TotalDeaths = 0;
};

/**
 * UDragonSlayerSaveGame
 * Save game object containing all progression data.
 */
UCLASS()
class DRAGONSLAYER_API UDragonSlayerSaveGame : public UPlatformerSaveGame
{
	GENERATED_BODY()

public:
	UDragonSlayerSaveGame();

	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Progression")
	FPlayerProgressionData ProgressionData;
};
