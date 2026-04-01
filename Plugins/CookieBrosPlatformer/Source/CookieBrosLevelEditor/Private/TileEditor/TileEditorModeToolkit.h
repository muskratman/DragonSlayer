// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class STilePaletteWidget;

/**
 * FTileEditorModeToolkit
 * Provides the Slate UI panel for the Tile Editor mode.
 * Hosts the tile palette widget, layer selector, and tool buttons.
 */
class FTileEditorModeToolkit : public FModeToolkit
{
public:
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) override;

	// ----- FModeToolkit interface -----
	virtual FName GetToolkitFName() const override { return FName("TileEditorModeToolkit"); }
	virtual FText GetBaseToolkitName() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override;

	/** Get the palette widget. */
	TSharedPtr<STilePaletteWidget> GetPaletteWidget() const { return PaletteWidget; }

private:
	/** The main toolkit content widget. */
	TSharedPtr<SWidget> ToolkitContent;

	/** The tile palette widget. */
	TSharedPtr<STilePaletteWidget> PaletteWidget;

	/** Build the full toolkit UI. */
	TSharedRef<SWidget> BuildToolkitUI();
};
