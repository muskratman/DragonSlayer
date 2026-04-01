// Copyright Epic Games, Inc. All Rights Reserved.


#include "PlatformerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "PlatformerUI.h"
#include "Core/UI/DragonSlayerHUD.h"
#include "PlatformerPlayerController.h"

APlatformerGameMode::APlatformerGameMode()
{
	HUDClass = ADragonSlayerHUD::StaticClass();
	PlayerControllerClass = APlatformerPlayerController::StaticClass();
}

void APlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!UserInterfaceClass)
	{
		return;
	}

	if (APlayerController* OwningPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		UserInterface = CreateWidget<UPlatformerUI>(OwningPlayer, UserInterfaceClass);
		check(UserInterface);
	}
}

void APlatformerGameMode::ProcessPickup()
{
	if (!UserInterface)
	{
		return;
	}

	// if this is the first pickup we collect, show the UI
	if (GetPickupsCollected() == 1)
	{
		UserInterface->AddToViewport(0);
	}

	// update the pickups counter on the UI
	UserInterface->UpdatePickups(GetPickupsCollected());
}

void APlatformerGameMode::HandlePickupCollected(APlatformerPickup* Pickup, ACharacter* Collector)
{
	ProcessPickup();
}
