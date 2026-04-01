// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"

/**
 * FTileEditorCommands
 * Defines all keyboard shortcuts and toolbar commands for the Tile Editor mode.
 */
class FTileEditorCommands : public TCommands<FTileEditorCommands>
{
public:
	FTileEditorCommands()
		: TCommands<FTileEditorCommands>(
			TEXT("TileEditor"),
			NSLOCTEXT("Contexts", "TileEditor", "Tile Editor 2.5D"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	// ----- Tool activation -----
	TSharedPtr<FUICommandInfo> ActivatePaintTool;
	TSharedPtr<FUICommandInfo> ActivateEraseTool;
	TSharedPtr<FUICommandInfo> ActivateSelectTool;

	// ----- Tile manipulation -----
	TSharedPtr<FUICommandInfo> RotateTileCW;
	TSharedPtr<FUICommandInfo> RotateTileCCW;

	// ----- Layer switching -----
	TSharedPtr<FUICommandInfo> SelectLayer1;
	TSharedPtr<FUICommandInfo> SelectLayer2;
	TSharedPtr<FUICommandInfo> SelectLayer3;
	TSharedPtr<FUICommandInfo> SelectLayer4;
};
