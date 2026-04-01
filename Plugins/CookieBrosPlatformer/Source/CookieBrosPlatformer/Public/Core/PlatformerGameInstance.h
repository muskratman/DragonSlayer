// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/PlatformerSaveProgressService.h"
#include "PlatformerGameInstance.generated.h"

class UPlatformerSaveGame;
class UPlatformerSaveGameSettings;

/**
 * Generic platformer game instance shell with startup save initialization.
 */
UCLASS(BlueprintType, Blueprintable, Config="Game")
class COOKIEBROSPLATFORMER_API UPlatformerGameInstance : public UGameInstance, public IPlatformerSaveProgressService
{
	GENERATED_BODY()

public:
	UPlatformerGameInstance();

	virtual void Init() override;
	virtual void OnStart() override;
	virtual bool SavePlatformerProgress_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Setup")
	bool IsGameInstanceSetupCompleted() const { return bGameInstanceSetupCompleted; }

	UFUNCTION(BlueprintCallable, Category="Save")
	virtual bool SaveProgress();

	UFUNCTION(BlueprintCallable, Category="Save")
	virtual bool LoadProgress(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Save")
	virtual bool SaveSettings();

	UFUNCTION(BlueprintCallable, Category="Save")
	virtual bool LoadSettings(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Setup")
	void OnGameInstanceSetuped();

	FORCEINLINE UPlatformerSaveGame* GetPlatformerSave() const { return CurrentSave; }
	FORCEINLINE UPlatformerSaveGameSettings* GetSettingsSave() const { return SettingsSave; }

protected:
	virtual void OnGameInstanceSetuped_Implementation();
	virtual void CapturePlatformerProgress(UPlatformerSaveGame* SaveGame) const;
	virtual bool ValidateLoadedPlatformerSave(UPlatformerSaveGame* SaveGame) const;

	bool InitializeSaveGames();
	bool EnsureProgressSave(int32 SlotIndex);
	bool EnsureSettingsSave(int32 SlotIndex);

	UPROPERTY(BlueprintReadOnly, Category="Save")
	TObjectPtr<UPlatformerSaveGame> CurrentSave;

	UPROPERTY(BlueprintReadOnly, Category="Save")
	TObjectPtr<UPlatformerSaveGameSettings> SettingsSave;

	UPROPERTY(EditDefaultsOnly, Category="Save")
	FString SaveSlotName = TEXT("SaveGamePlatformer_1");

	UPROPERTY(EditDefaultsOnly, Category="Save")
	FString SettingsSlotName = TEXT("SaveSettingsSlot_1");

	UPROPERTY(EditDefaultsOnly, Category="Save", meta=(ClampMin=0))
	int32 SaveUserIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category="Save")
	TSubclassOf<UPlatformerSaveGame> PlatformerSaveClass;

	UPROPERTY(EditDefaultsOnly, Category="Save")
	TSubclassOf<UPlatformerSaveGameSettings> SettingsSaveClass;

private:
	bool bGameInstanceSetupCompleted = false;
	bool bGameInstanceSetupBroadcasted = false;
};
