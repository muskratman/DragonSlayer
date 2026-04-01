// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlatformerUI.generated.h"

/**
 *  Simple platformer game UI
 *  Displays and manages a pickup counter
 */
UCLASS(abstract)
class UPlatformerUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/** Update the widget's pickup counter */
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void UpdatePickups(int32 Amount);
};
