// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/TileEditorModeToolkit.h"
#include "TileEditor/TileEditorMode.h"
#include "TileEditor/TileEditorCommands.h"
#include "TileEditor/STilePaletteWidget.h"
#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"
#include "TileMap/TileSetAsset.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Styling/AppStyle.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "TileEditorModeToolkit"

void FTileEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	ToolkitContent = BuildToolkitUI();
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

FText FTileEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("ToolkitName", "Tile Editor 2.5D");
}

TSharedPtr<SWidget> FTileEditorModeToolkit::GetInlineContent() const
{
	return ToolkitContent;
}

TSharedRef<SWidget> FTileEditorModeToolkit::BuildToolkitUI()
{
	// Get the owning mode to access its state
	UTileEditorMode* TileMode = Cast<UTileEditorMode>(
		GLevelEditorModeTools().GetActiveScriptableMode(UTileEditorMode::EM_TileEditorModeId));

	// Resolve the tile set for the palette
	UTileSetAsset* TileSet = nullptr;
	if (TileMode && TileMode->ActiveTileMapActor && TileMode->ActiveTileMapActor->TileMapAsset)
	{
		TileSet = TileMode->ActiveTileMapActor->TileMapAsset->TileSet.LoadSynchronous();
	}

	// Build palette widget
	SAssignNew(PaletteWidget, STilePaletteWidget)
		.TileSet(TileSet)
		.OnTileSelected_Lambda([TileMode](FName TileID)
		{
			if (TileMode)
			{
				TileMode->OnPaletteTileSelected(TileID);
			}
		});

	const FTileEditorCommands& Cmds = FTileEditorCommands::Get();

	return SNew(SScrollBox)

		// === Section: Tools ===
		+ SScrollBox::Slot()
		.Padding(4.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ToolsHeader", "Tools"))
				.Font(FAppStyle::GetFontStyle("BoldFont"))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("PaintBtn", "Paint (B)"))
					.OnClicked_Lambda([this]()
					{
						GetToolkitCommands()->ExecuteAction(FTileEditorCommands::Get().ActivatePaintTool.ToSharedRef());
						return FReply::Handled();
					})
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("EraseBtn", "Erase (E)"))
					.OnClicked_Lambda([this]()
					{
						GetToolkitCommands()->ExecuteAction(FTileEditorCommands::Get().ActivateEraseTool.ToSharedRef());
						return FReply::Handled();
					})
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("SelectBtn", "Select (S)"))
					.OnClicked_Lambda([this]()
					{
						GetToolkitCommands()->ExecuteAction(FTileEditorCommands::Get().ActivateSelectTool.ToSharedRef());
						return FReply::Handled();
					})
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f, 4.0f)
			[
				SNew(SSeparator)
			]

			// === Section: Layers ===
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LayersHeader", "Layers"))
				.Font(FAppStyle::GetFontStyle("BoldFont"))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)
				[
					SNew(SButton).Text(LOCTEXT("L1", "BG (1)"))
					.OnClicked_Lambda([TileMode]() { if(TileMode) TileMode->SetActiveLayer(0); return FReply::Handled(); })
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)
				[
					SNew(SButton).Text(LOCTEXT("L2", "Ground (2)"))
					.OnClicked_Lambda([TileMode]() { if(TileMode) TileMode->SetActiveLayer(1); return FReply::Handled(); })
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)
				[
					SNew(SButton).Text(LOCTEXT("L3", "FG (3)"))
					.OnClicked_Lambda([TileMode]() { if(TileMode) TileMode->SetActiveLayer(2); return FReply::Handled(); })
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)
				[
					SNew(SButton).Text(LOCTEXT("L4", "Special (4)"))
					.OnClicked_Lambda([TileMode]() { if(TileMode) TileMode->SetActiveLayer(3); return FReply::Handled(); })
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f, 4.0f)
			[
				SNew(SSeparator)
			]

			// === Section: Tile Palette ===
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("PaletteHeader", "Tile Palette"))
				.Font(FAppStyle::GetFontStyle("BoldFont"))
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(2.0f)
			[
				PaletteWidget.ToSharedRef()
			]
		];
}

#undef LOCTEXT_NAMESPACE
