#include "Core/DragonSlayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/DragonSlayerSaveGame.h"

UDragonSlayerGameInstance::UDragonSlayerGameInstance()
{
	PlatformerSaveClass = UDragonSlayerSaveGame::StaticClass();
}

bool UDragonSlayerGameInstance::ValidateLoadedPlatformerSave(UPlatformerSaveGame* SaveGame) const
{
	return Cast<UDragonSlayerSaveGame>(SaveGame) != nullptr;
}

void UDragonSlayerGameInstance::OnGameInstanceSetuped_Implementation()
{
	Super::OnGameInstanceSetuped_Implementation();

	const FName CurrentLevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
	if (!StartupTransitLevelName.IsNone() && CurrentLevelName != StartupTransitLevelName)
	{
		return;
	}

	if (!StartupCompletedLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, StartupCompletedLevelName);
	}
}

UDragonSlayerSaveGame* UDragonSlayerGameInstance::GetDragonSlayerSave() const
{
	return Cast<UDragonSlayerSaveGame>(GetPlatformerSave());
}
