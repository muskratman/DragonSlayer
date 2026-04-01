// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/STilePaletteWidget.h"
#include "TileMap/TileSetAsset.h"
#include "TileMap/TileMapTypes.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/AppStyle.h"
#include "Engine/Texture2D.h"

#define LOCTEXT_NAMESPACE "STilePaletteWidget"

void STilePaletteWidget::Construct(const FArguments& InArgs)
{
	CachedTileSet = InArgs._TileSet;
	OnTileSelectedDelegate = InArgs._OnTileSelected;

	RefreshTileList();

	ChildSlot
	[
		SNew(SVerticalBox)

		// Search bar
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(4.0f)
		[
			SNew(SSearchBox)
			.HintText(LOCTEXT("SearchHint", "Filter tiles..."))
			.OnTextChanged(this, &STilePaletteWidget::OnFilterTextChanged)
		]

		// Tile grid
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SAssignNew(TileViewWidget, STileView<TSharedPtr<FName>>)
			.ListItemsSource(&TileItems)
			.OnGenerateTile(this, &STilePaletteWidget::OnGenerateTileRow)
			.OnSelectionChanged(this, &STilePaletteWidget::OnTileSelectionChanged)
			.ItemWidth(80)
			.ItemHeight(100)
		]
	];
}

void STilePaletteWidget::SetTileSet(UTileSetAsset* NewTileSet)
{
	CachedTileSet = NewTileSet;
	SelectedTileID = NAME_None;
	RefreshTileList();
}

void STilePaletteWidget::SetSelectedTileID(FName TileID)
{
	SelectedTileID = TileID;
	// Find and select in tile view
	for (const TSharedPtr<FName>& Item : TileItems)
	{
		if (Item.IsValid() && *Item == TileID)
		{
			TileViewWidget->SetSelection(Item);
			break;
		}
	}
}

void STilePaletteWidget::RefreshTileList()
{
	TileItems.Empty();

	UTileSetAsset* TileSet = CachedTileSet.Get();
	if (!TileSet) return;

	for (const FTileDefinition& Def : TileSet->Tiles)
	{
		// Apply text filter
		if (!FilterText.IsEmpty())
		{
			const FString DisplayStr = Def.DisplayName.IsEmpty()
				? Def.TileID.ToString()
				: Def.DisplayName.ToString();
			if (!DisplayStr.Contains(FilterText))
			{
				continue;
			}
		}
		TileItems.Add(MakeShared<FName>(Def.TileID));
	}

	if (TileViewWidget.IsValid())
	{
		TileViewWidget->RequestListRefresh();
	}
}

TSharedRef<ITableRow> STilePaletteWidget::OnGenerateTileRow(
	TSharedPtr<FName> Item,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	FText Label = FText::FromName(Item.IsValid() ? *Item : NAME_None);
	const FSlateBrush* TileBrush = FAppStyle::GetBrush("Icons.Help"); // Default fallback

	// Try to get the palette icon from the tile definition
	if (Item.IsValid() && CachedTileSet.IsValid())
	{
		if (const FTileDefinition* Def = CachedTileSet->FindTileByID(*Item))
		{
			Label = Def->DisplayName.IsEmpty()
				? FText::FromName(Def->TileID)
				: Def->DisplayName;

			// TODO: Create a dynamic brush from Def->PaletteIcon if loaded.
			// For now we use the default brush as placeholder.
		}
	}

	return SNew(STableRow<TSharedPtr<FName>>, OwnerTable)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(4.0f)
		[
			SNew(SImage)
			.Image(TileBrush)
			.DesiredSizeOverride(FVector2D(48, 48))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(Label)
			.Justification(ETextJustify::Center)
			.Font(FAppStyle::GetFontStyle("SmallFont"))
		]
	];
}

void STilePaletteWidget::OnTileSelectionChanged(TSharedPtr<FName> Item, ESelectInfo::Type SelectInfo)
{
	if (Item.IsValid())
	{
		SelectedTileID = *Item;
		OnTileSelectedDelegate.ExecuteIfBound(SelectedTileID);
	}
}

void STilePaletteWidget::OnFilterTextChanged(const FText& NewText)
{
	FilterText = NewText.ToString();
	RefreshTileList();
}

#undef LOCTEXT_NAMESPACE
