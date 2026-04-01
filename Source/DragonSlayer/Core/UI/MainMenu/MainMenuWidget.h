#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UComboBoxString;
class UButton;

/**
 * Base class for the Main Menu widget. Layout is authored in WidgetBlueprint.
 */
UCLASS(Abstract)
class DRAGONSLAYER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FString LevelsRootPath = TEXT("/Game/PlatformerLevels");

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> LevelComboBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

	UFUNCTION()
	void OnStartGameClicked();

private:
	void PopulateLevels();
};
