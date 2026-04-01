#include "MainMenuWidget.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameClicked);
	}

	PopulateLevels();
}

void UMainMenuWidget::OnStartGameClicked()
{
	if (LevelComboBox && LevelComboBox->GetSelectedOption().Len() > 0)
	{
		const FString SelectedLevel = LevelComboBox->GetSelectedOption();
		const FString NormalizedLevelsRootPath = LevelsRootPath.EndsWith(TEXT("/"))
			? LevelsRootPath.LeftChop(1)
			: LevelsRootPath;
		const FString LevelPath = FString::Printf(TEXT("%s/%s"), *NormalizedLevelsRootPath, *SelectedLevel);

		UGameplayStatics::OpenLevel(this, FName(*LevelPath));
	}
}

void UMainMenuWidget::PopulateLevels()
{
	if (!LevelComboBox)
	{
		return;
	}

	LevelComboBox->ClearOptions();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	const FString NormalizedLevelsRootPath = LevelsRootPath.EndsWith(TEXT("/"))
		? LevelsRootPath.LeftChop(1)
		: LevelsRootPath;

	TArray<FAssetData> AssetDataArray;
	AssetRegistry.GetAssetsByPath(FName(*NormalizedLevelsRootPath), AssetDataArray, /*bRecursive=*/ true);

	for (const FAssetData& AssetData : AssetDataArray)
	{
		if (AssetData.AssetClassPath == UWorld::StaticClass()->GetClassPathName())
		{
			LevelComboBox->AddOption(AssetData.AssetName.ToString());
		}
	}

	if (LevelComboBox->GetOptionCount() > 0)
	{
		LevelComboBox->SetSelectedIndex(0);
	}
}
