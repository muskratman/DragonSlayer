#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

class UMainMenuWidget;

/**
 * HUD for the Main Menu. Creates and manages the MainMenuWidget.
 */
UCLASS()
class DRAGONSLAYER_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMainMenuHUD();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidgetInstance;
};
