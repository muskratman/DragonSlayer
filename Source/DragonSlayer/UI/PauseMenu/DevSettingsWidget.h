#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "DevSettingsWidget.generated.h"

UCLASS(Abstract)
class DRAGONSLAYER_API UDevSettingsWidget : public UUserWidget {
  GENERATED_BODY()

protected:
  virtual void NativeOnInitialized() override;

  // Base class for DevSettings. Sliders and bindings to UDeveloperSettings will
  // be added here
};
