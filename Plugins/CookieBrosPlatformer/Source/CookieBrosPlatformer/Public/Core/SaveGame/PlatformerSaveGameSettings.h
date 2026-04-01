// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlatformerSaveGameSettings.generated.h"

/**
 * Generic settings save shell for platformer projects.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API UPlatformerSaveGameSettings : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Audio", meta=(ClampMin=0.0, ClampMax=1.0))
	float MasterVolume = 1.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Audio", meta=(ClampMin=0.0, ClampMax=1.0))
	float MusicVolume = 1.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Audio", meta=(ClampMin=0.0, ClampMax=1.0))
	float SfxVolume = 1.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Graphics")
	bool bEnableVSync = true;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Graphics", meta=(ClampMin=0.5, ClampMax=2.0))
	float UIScale = 1.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Input")
	bool bInvertHorizontalLook = false;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category="Input")
	bool bInvertVerticalLook = false;
};
