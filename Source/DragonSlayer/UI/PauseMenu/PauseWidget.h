#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "PauseWidget.generated.h"

class UButton;

UCLASS(Abstract)
class DRAGONSLAYER_API UPauseWidget : public UUserWidget {
  GENERATED_BODY()

protected:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UButton> Btn_Resume;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UButton> Btn_MainMenu;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UButton> Btn_Settings;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UButton> Btn_DevSettings;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UButton> Btn_Exit;

private:
  UFUNCTION()
  void OnResumeClicked();

  UFUNCTION()
  void OnMainMenuClicked();

  UFUNCTION()
  void OnSettingsClicked();

  UFUNCTION()
  void OnDevSettingsClicked();

  UFUNCTION()
  void OnExitClicked();
};
