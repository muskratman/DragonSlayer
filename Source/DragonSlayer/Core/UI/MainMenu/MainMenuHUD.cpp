#include "MainMenuHUD.h"
#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Logging/LogMacros.h"

AMainMenuHUD::AMainMenuHUD()
{
}

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	if (!MainMenuWidgetInstance && MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuWidgetClass is not set on %s"), *GetName());
	}
}
