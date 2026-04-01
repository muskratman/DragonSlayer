#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DragonSlayerHUD.generated.h"

class UPauseWidget;

UCLASS()
class DRAGONSLAYER_API ADragonSlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	ADragonSlayerHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HidePauseMenu();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPauseWidget> PauseWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UPauseWidget> PauseWidgetInstance;
};
