// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STileView.h"

class UTileSetAsset;
struct FTileDefinition;

DECLARE_DELEGATE_OneParam(FOnTileSelected, FName /*TileID*/);

/**
 * STilePaletteWidget
 * Slate widget displaying a grid of tile thumbnails from a UTileSetAsset.
 * Supports search/filter and selection notification.
 */
class STilePaletteWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STilePaletteWidget) {}
		SLATE_ARGUMENT(UTileSetAsset*, TileSet)
		SLATE_EVENT(FOnTileSelected, OnTileSelected)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Update the tile set data source. */
	void SetTileSet(UTileSetAsset* NewTileSet);

	/** Get the currently selected tile ID. */
	FName GetSelectedTileID() const { return SelectedTileID; }

	/** Set selection programmatically. */
	void SetSelectedTileID(FName TileID);

private:
	/** Cached tile set reference. */
	TWeakObjectPtr<UTileSetAsset> CachedTileSet;

	/** Items for the tile view. */
	TArray<TSharedPtr<FName>> TileItems;

	/** Currently selected tile. */
	FName SelectedTileID;

	/** Selection callback. */
	FOnTileSelected OnTileSelectedDelegate;

	/** Filter text. */
	FString FilterText;

	/** Tile view widget. */
	TSharedPtr<STileView<TSharedPtr<FName>>> TileViewWidget;

	/** Rebuild the TileItems list from the tile set (applying filter). */
	void RefreshTileList();

	/** Generate a single tile entry widget. */
	TSharedRef<ITableRow> OnGenerateTileRow(TSharedPtr<FName> Item,
		const TSharedRef<STableViewBase>& OwnerTable);

	/** Handle selection change in the tile view. */
	void OnTileSelectionChanged(TSharedPtr<FName> Item, ESelectInfo::Type SelectInfo);

	/** Handle filter text change. */
	void OnFilterTextChanged(const FText& NewText);
};
