// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/TileMapActorDetails.h"
#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "ScopedTransaction.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "TileMapActorDetails"

TSharedRef<IDetailCustomization> FTileMapActorDetails::MakeInstance()
{
	return MakeShareable(new FTileMapActorDetails);
}

void FTileMapActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Get the actor being customized
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	if (Objects.Num() == 1 && Objects[0].IsValid())
	{
		CachedActor = Cast<ATileMapActor>(Objects[0].Get());
	}

	// ----- TileMap Actions category -----
	IDetailCategoryBuilder& ActionsCategory = DetailBuilder.EditCategory(
		"TileMap Actions", LOCTEXT("ActionsCat", "TileMap Actions"), ECategoryPriority::Important);

	// Rebuild button
	ActionsCategory.AddCustomRow(LOCTEXT("RebuildRow", "Rebuild"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("RebuildLabel", "Rebuild Instances"))
		.Font(FAppStyle::GetFontStyle("SmallFont"))
	]
	.ValueContent()
	.MaxDesiredWidth(200.0f)
	[
		SNew(SButton)
		.Text(LOCTEXT("RebuildBtn", "Rebuild All"))
		.HAlign(HAlign_Center)
		.OnClicked(FOnClicked::CreateRaw(this, &FTileMapActorDetails::OnRebuildClicked))
	];

	// Ensure Default Layers button
	ActionsCategory.AddCustomRow(LOCTEXT("DefaultLayersRow", "Default Layers"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DefaultLayersLabel", "Layer Setup"))
		.Font(FAppStyle::GetFontStyle("SmallFont"))
	]
	.ValueContent()
	.MaxDesiredWidth(200.0f)
	[
		SNew(SButton)
		.Text(LOCTEXT("DefaultLayersBtn", "Create Default Layers"))
		.HAlign(HAlign_Center)
		.OnClicked(FOnClicked::CreateRaw(this, &FTileMapActorDetails::OnEnsureDefaultLayersClicked))
	];

	// ----- Import / Export category -----
	IDetailCategoryBuilder& IOCategory = DetailBuilder.EditCategory(
		"TileMap IO", LOCTEXT("IOCat", "Import / Export"), ECategoryPriority::Default);

	IOCategory.AddCustomRow(LOCTEXT("ExportRow", "Export"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ExportLabel", "JSON Export"))
		.Font(FAppStyle::GetFontStyle("SmallFont"))
	]
	.ValueContent()
	.MaxDesiredWidth(200.0f)
	[
		SNew(SButton)
		.Text(LOCTEXT("ExportBtn", "Export to JSON"))
		.HAlign(HAlign_Center)
		.OnClicked(FOnClicked::CreateRaw(this, &FTileMapActorDetails::OnExportJsonClicked))
	];

	IOCategory.AddCustomRow(LOCTEXT("ImportRow", "Import"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ImportLabel", "JSON Import"))
		.Font(FAppStyle::GetFontStyle("SmallFont"))
	]
	.ValueContent()
	.MaxDesiredWidth(200.0f)
	[
		SNew(SButton)
		.Text(LOCTEXT("ImportBtn", "Import from JSON"))
		.HAlign(HAlign_Center)
		.OnClicked(FOnClicked::CreateRaw(this, &FTileMapActorDetails::OnImportJsonClicked))
	];
}

// ============================================================================
// Button handlers
// ============================================================================

FReply FTileMapActorDetails::OnRebuildClicked()
{
	if (CachedActor.IsValid())
	{
		CachedActor->RebuildAllInstances();
	}
	return FReply::Handled();
}

FReply FTileMapActorDetails::OnExportJsonClicked()
{
	if (!CachedActor.IsValid() || !CachedActor->TileMapAsset) return FReply::Handled();

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform) return FReply::Handled();

	TArray<FString> OutFiles;
	const bool bOpened = DesktopPlatform->SaveFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		TEXT("Export TileMap to JSON"),
		FPaths::ProjectDir(),
		TEXT("tilemap.json"),
		TEXT("JSON Files (*.json)|*.json"),
		0,
		OutFiles
	);

	if (bOpened && OutFiles.Num() > 0)
	{
		CachedActor->TileMapAsset->ExportToJson(OutFiles[0]);
	}

	return FReply::Handled();
}

FReply FTileMapActorDetails::OnImportJsonClicked()
{
	if (!CachedActor.IsValid() || !CachedActor->TileMapAsset) return FReply::Handled();

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform) return FReply::Handled();

	TArray<FString> OutFiles;
	const bool bOpened = DesktopPlatform->OpenFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		TEXT("Import TileMap from JSON"),
		FPaths::ProjectDir(),
		TEXT(""),
		TEXT("JSON Files (*.json)|*.json"),
		0,
		OutFiles
	);

	if (bOpened && OutFiles.Num() > 0)
	{
		FScopedTransaction Transaction(LOCTEXT("ImportTileMap", "Import TileMap from JSON"));
		CachedActor->TileMapAsset->Modify();

		if (CachedActor->TileMapAsset->ImportFromJson(OutFiles[0]))
		{
			CachedActor->RebuildAllInstances();
		}
	}

	return FReply::Handled();
}

FReply FTileMapActorDetails::OnEnsureDefaultLayersClicked()
{
	if (!CachedActor.IsValid() || !CachedActor->TileMapAsset) return FReply::Handled();

	FScopedTransaction Transaction(LOCTEXT("CreateDefaultLayers", "Create Default Layers"));
	CachedActor->TileMapAsset->Modify();
	CachedActor->TileMapAsset->EnsureDefaultLayers();

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
