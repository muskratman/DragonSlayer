// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileMap/TileMapTypes.h"
#include "TileSetAsset.generated.h"

/**
 * UTileSetAsset
 * A DataAsset containing a palette of tile definitions.
 * Referenced by UTileMapAsset to look up meshes, icons, sizes, etc.
 */
UCLASS(BlueprintType)
class COOKIEBROSPLATFORMER_API UTileSetAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Human-readable name for this tile set. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TileSet")
	FName TileSetName;

	/** All tile definitions in this set. Each must have a unique TileID. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TileSet")
	TArray<FTileDefinition> Tiles;

	// ----- Lookup helpers -----

	/** Find a tile definition by its unique ID. Returns nullptr if not found. */
	const FTileDefinition* FindTileByID(FName TileID) const;

	/** Get valid tile IDs. */
	UFUNCTION(BlueprintPure, Category="TileSet")
	TArray<FName> GetAllTileIDs() const;

	/** Check whether a given ID exists in this set. */
	UFUNCTION(BlueprintPure, Category="TileSet")
	bool ContainsTileID(FName TileID) const;

	// ----- UPrimaryDataAsset overrides -----
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
