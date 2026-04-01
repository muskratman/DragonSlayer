// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/TileEditorCommands.h"

#define LOCTEXT_NAMESPACE "TileEditorCommands"

void FTileEditorCommands::RegisterCommands()
{
	// Tool activation
	UI_COMMAND(ActivatePaintTool, "Paint", "Paint tiles onto the grid",
		EUserInterfaceActionType::RadioButton, FInputChord(EKeys::B));

	UI_COMMAND(ActivateEraseTool, "Erase", "Erase tiles from the grid",
		EUserInterfaceActionType::RadioButton, FInputChord(EKeys::E));

	UI_COMMAND(ActivateSelectTool, "Select", "Select tiles for inspection or batch operations",
		EUserInterfaceActionType::RadioButton, FInputChord(EKeys::S));

	// Tile manipulation
	UI_COMMAND(RotateTileCW, "Rotate CW", "Rotate selected tile 90 degrees clockwise",
		EUserInterfaceActionType::Button, FInputChord(EKeys::RightBracket));

	UI_COMMAND(RotateTileCCW, "Rotate CCW", "Rotate selected tile 90 degrees counter-clockwise",
		EUserInterfaceActionType::Button, FInputChord(EKeys::LeftBracket));

	// Layer switching
	UI_COMMAND(SelectLayer1, "Layer 1", "Switch to layer 1 (Background)",
		EUserInterfaceActionType::RadioButton, FInputChord(EKeys::One));

	UI_COMMAND(SelectLayer2, "Layer 2", "Switch to layer 2 (Ground)",
		EUserInterfaceActionType::RadioButton, FInputChord(EKeys::Two));

	UI_COMMAND(SelectLayer3, "Layer 3", "Switch to layer 3 (Foreground)",
		EUserInterfaceActionType::RadioButton, FInputChord(EKeys::Three));

	UI_COMMAND(SelectLayer4, "Layer 4", "Switch to layer 4 (Special)",
		EUserInterfaceActionType::RadioButton, FInputChord(EKeys::Four));
}

#undef LOCTEXT_NAMESPACE
