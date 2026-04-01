#include "Core/UI/DragonSlayerHUD.h"
#include "UI/PauseMenu/PauseWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

ADragonSlayerHUD::ADragonSlayerHUD()
{
}

void ADragonSlayerHUD::TogglePauseMenu()
{
	if (PauseWidgetInstance && PauseWidgetInstance->IsInViewport())
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void ADragonSlayerHUD::ShowPauseMenu()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	if (!PauseWidgetInstance && PauseWidgetClass)
	{
		PauseWidgetInstance = CreateWidget<UPauseWidget>(PC, PauseWidgetClass);
	}

	if (PauseWidgetInstance && !PauseWidgetInstance->IsInViewport())
	{
		PauseWidgetInstance->AddToViewport(10); // High Z-order to be on top

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseWidgetInstance->TakeWidget());
		InputMode.SetHideCursorDuringCapture(false);
		
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		PC->SetPause(true);
	}
}

void ADragonSlayerHUD::HidePauseMenu()
{
	if (PauseWidgetInstance && PauseWidgetInstance->IsInViewport())
	{
		PauseWidgetInstance->RemoveFromParent();

		if (APlayerController* PC = GetOwningPlayerController())
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
			PC->SetPause(false);
		}
	}
}
