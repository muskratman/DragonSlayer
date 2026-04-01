// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "TileEditorMode.generated.h"

class ATileMapActor;
class UTileSetAsset;

/**
 * UTileEditorMode
 * Custom UEdMode for the 2.5D tile-based level editor.
 * Registers Paint/Erase/Select tools via the Interactive Tools Framework.
 * Renders grid overlay and ghost preview in the viewport.
 */
UCLASS()
class UTileEditorMode : public UEdMode
{
	GENERATED_BODY()

public:
	/** Mode identifier. */
	static const FEditorModeID EM_TileEditorModeId;

	/** Tool identifiers. */
	static const FString ToolName_Paint;
	static const FString ToolName_Erase;
	static const FString ToolName_Select;

	UTileEditorMode();

	// ----- UEdMode interface -----
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CreateToolkit() override;
	virtual bool UsesToolkits() const override { return true; }
	virtual bool IsSelectionAllowed(AActor* InActor, bool bInSelection) const override;
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;

	// ----- State -----

	/** Currently targeted tile map actor (auto-detected or user-selected). */
	UPROPERTY(Transient)
	TObjectPtr<ATileMapActor> ActiveTileMapActor;

	/** Currently selected tile ID (set by palette). */
	FName SelectedTileID;

	/** Current rotation step (0-3). */
	int32 CurrentRotationStep = 0;

	/** Active layer index. */
	int32 ActiveLayerIndex = 1; // Default = Ground

	/** Rotate current tile CW. */
	void RotateCW();

	/** Rotate current tile CCW. */
	void RotateCCW();

	/** Set active layer. */
	void SetActiveLayer(int32 LayerIndex);

	/** Called when a tile is selected in the palette. */
	void OnPaletteTileSelected(FName TileID);

private:
	/** Find a TileMapActor in the current level. */
	ATileMapActor* FindTileMapActorInLevel() const;

	/** Bind editor commands to actions. */
	void BindCommands();
};
