// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TileMap/TileMapTypes.h"
#include "CBPTileMapBlueprintLibrary.generated.h"

class ATileMapActor;

/**
 * UCBPTileMapLibrary
 * Static Blueprint-callable helpers for runtime tile map manipulation.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API UCBPTileMapLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Place a tile at the given cell on the specified layer.
	 * Overwrites any existing tile at that cell.
	 * @return true if the tile was placed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="TileMap", meta=(DisplayName="Add Tile"))
	static bool AddTile(ATileMapActor* Actor, int32 LayerIndex,
	                    FIntPoint Cell, FName TileID, int32 RotationStep = 0);

	/**
	 * Remove a tile at the given cell on the specified layer.
	 * @return true if a tile was actually removed.
	 */
	UFUNCTION(BlueprintCallable, Category="TileMap", meta=(DisplayName="Remove Tile"))
	static bool RemoveTile(ATileMapActor* Actor, int32 LayerIndex, FIntPoint Cell);

	/**
	 * Get the tile instance at the given cell. Returns a default (invalid) instance if empty.
	 */
	UFUNCTION(BlueprintPure, Category="TileMap", meta=(DisplayName="Get Tile At"))
	static FTileInstance GetTileAt(ATileMapActor* Actor, int32 LayerIndex, FIntPoint Cell);

	/**
	 * Check whether a tile exists at the given cell.
	 */
	UFUNCTION(BlueprintPure, Category="TileMap", meta=(DisplayName="Has Tile At"))
	static bool HasTileAt(ATileMapActor* Actor, int32 LayerIndex, FIntPoint Cell);

	/**
	 * Remove all tiles from a layer.
	 */
	UFUNCTION(BlueprintCallable, Category="TileMap", meta=(DisplayName="Clear Layer"))
	static void ClearLayer(ATileMapActor* Actor, int32 LayerIndex);

	/**
	 * Get the map dimensions in cells.
	 */
	UFUNCTION(BlueprintPure, Category="TileMap", meta=(DisplayName="Get Map Size"))
	static FIntPoint GetMapSize(ATileMapActor* Actor);

	/**
	 * Convert a world position to a cell coordinate.
	 */
	UFUNCTION(BlueprintPure, Category="TileMap", meta=(DisplayName="World To Cell"))
	static FIntPoint WorldToCell(ATileMapActor* Actor, FVector WorldPos);
};
