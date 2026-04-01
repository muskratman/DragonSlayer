#pragma once

#include "CoreMinimal.h"
#include "Core/PlatformerGameInstance.h"
#include "DragonSlayerGameInstance.generated.h"

class UDragonSlayerSaveGame;
class UPlatformerSaveGame;

/**
 * UDragonSlayerGameInstance
 * Project-local game instance that binds the concrete save type and startup transition.
 */
UCLASS(BlueprintType, Blueprintable, Config="Game")
class DRAGONSLAYER_API UDragonSlayerGameInstance : public UPlatformerGameInstance
{
	GENERATED_BODY()

public:
	UDragonSlayerGameInstance();

	UFUNCTION(BlueprintCallable, Category="Save")
	UDragonSlayerSaveGame* GetDragonSlayerSave() const;

protected:
	virtual bool ValidateLoadedPlatformerSave(UPlatformerSaveGame* SaveGame) const override;
	virtual void OnGameInstanceSetuped_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category="Startup")
	FName StartupTransitLevelName = TEXT("StartupMap");

	UPROPERTY(EditDefaultsOnly, Category="Startup")
	FName StartupCompletedLevelName = TEXT("MainMenuMap");
};
