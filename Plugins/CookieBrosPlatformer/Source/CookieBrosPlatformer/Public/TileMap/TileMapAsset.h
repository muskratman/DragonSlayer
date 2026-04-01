// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileMap/TileMapTypes.h"
#include "TileMapAsset.generated.h"

class UTileSetAsset;

/**
 * UTileMapAsset
 * Stores the full tile map data: grid configuration and per-layer tile instances.
 * This is the serialization target for the level editor.
 */
UCLASS(BlueprintType)
class COOKIEBROSPLATFORMER_API UTileMapAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Human-readable map name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
	FName MapName;

	/** Map dimensions in grid cells: X = columns (world X), Y = rows (world Z). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map", meta=(ClampMin=1))
	FIntPoint MapSizeInCells = FIntPoint(64, 32);

	/** Size of one grid cell in world units. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map", meta=(ClampMin=10.0f))
	float CellSize = 100.0f;

	/** The tile set used by this map for looking up tile definitions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
	TSoftObjectPtr<UTileSetAsset> TileSet;

	/** Ordered list of layers (bottom-to-top visual order). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
	TArray<FTileMapLayer> Layers;

	// ----- Tile CRUD -----

	/** Add a tile to a layer. Overwrites if a tile already exists at that cell. */
	void AddTile(int32 LayerIndex, const FTileInstance& Tile);

	/** Remove a tile at a given cell on a layer. Returns true if a tile was removed. */
	bool RemoveTileAt(int32 LayerIndex, FIntPoint Cell);

	/** Get a tile at a given cell on a layer. Returns nullptr if empty. */
	const FTileInstance* GetTileAt(int32 LayerIndex, FIntPoint Cell) const;

	/** Check whether a tile exists at a given cell on a layer. */
	bool HasTileAt(int32 LayerIndex, FIntPoint Cell) const;

	/** Remove all tiles from a layer. */
	void ClearLayer(int32 LayerIndex);

	// ----- Coordinate helpers -----

	/** Convert a world position to a cell coordinate (snapped). */
	FIntPoint WorldToCell(const FVector& WorldPos) const;

	/** Convert a cell coordinate to world position (center of cell). */
	FVector CellToWorld(FIntPoint Cell, float LayerDepthY = 0.0f) const;

	/** Check whether a cell coordinate is within map bounds. */
	bool IsCellInBounds(FIntPoint Cell) const;

	// ----- JSON Import/Export -----

	/** Export the entire tile map to a JSON file. */
	void ExportToJson(const FString& FilePath) const;

	/** Import tile map data from a JSON file. Returns false on parse failure. */
	bool ImportFromJson(const FString& FilePath);

	// ----- UPrimaryDataAsset overrides -----
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// ----- Default layer factory -----

	/** Create default layers if none exist (Background, Ground, Foreground, Special). */
	void EnsureDefaultLayers();
};
