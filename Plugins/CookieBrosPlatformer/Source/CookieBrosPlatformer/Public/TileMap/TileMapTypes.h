// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TileMapTypes.generated.h"

/**
 * FTileDefinition
 * Describes a single tile type inside a UTileSetAsset.
 * Mesh + PaletteIcon define visuals; SizeInCells determines grid footprint.
 */
USTRUCT(BlueprintType)
struct COOKIEBROSPLATFORMER_API FTileDefinition
{
	GENERATED_BODY()

	/** Unique identifier within the tile set. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile")
	FName TileID;

	/** Human-readable name shown in the palette UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile")
	FText DisplayName;

	/** Static mesh used for rendering (ignored for special blocks). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile|Visual")
	TSoftObjectPtr<UStaticMesh> Mesh;

	/** 2D icon shown in the tile palette and used for special-block viewport gizmos. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile|Visual")
	TSoftObjectPtr<UTexture2D> PaletteIcon;

	/** Tile footprint in grid cells: X = width (along world X), Y = height (along world Z). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile|Shape", meta=(ClampMin=1, ClampMax=16))
	FIntPoint SizeInCells = FIntPoint(1, 1);

	/** Whether this tile generates collision geometry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile|Physics")
	bool bCollisionEnabled = true;

	/**
	 * Special blocks are rendered in the viewport as a PaletteIcon billboard
	 * instead of the full 3D mesh. Use for triggers, checkpoints, secrets, etc.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile|Type")
	bool bIsSpecialBlock = false;

	/** Gameplay tag identifying the special block's purpose (e.g. Trigger, Checkpoint). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile|Type", meta=(EditCondition="bIsSpecialBlock"))
	FGameplayTag SpecialType;

	bool operator==(const FTileDefinition& Other) const { return TileID == Other.TileID; }
};

/**
 * FTileInstance
 * A single placed tile within a layer. References FTileDefinition::TileID.
 */
USTRUCT(BlueprintType)
struct COOKIEBROSPLATFORMER_API FTileInstance
{
	GENERATED_BODY()

	/** References FTileDefinition::TileID in the owning UTileSetAsset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile")
	FName TileID;

	/** Grid position: X = column (world X), Y = row (world Z). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile")
	FIntPoint CellPosition = FIntPoint::ZeroValue;

	/**
	 * Discrete rotation around Y axis (depth axis in side-view).
	 * 0 = 0 deg, 1 = 90 deg, 2 = 180 deg, 3 = 270 deg.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile", meta=(ClampMin=0, ClampMax=3))
	int32 RotationStep = 0;

	/** Convert RotationStep to a world-space rotator (rotation around Y). */
	FRotator GetRotator() const
	{
		// Yaw = 0, Roll = 0, Pitch = RotationStep * 90 — but for Y-axis rotation
		// In UE coords, rotation around Y = Roll in some conventions; we use FRotator directly.
		return FRotator(0.0f, 0.0f, RotationStep * 90.0f);
	}
};

/**
 * FTileMapLayer
 * A named collection of tile instances that share a depth (Y-offset) in side-view.
 */
USTRUCT(BlueprintType)
struct COOKIEBROSPLATFORMER_API FTileMapLayer
{
	GENERATED_BODY()

	/** Layer identifier (e.g. "Background", "Ground", "Foreground", "Special"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layer")
	FName LayerName;

	/** Y-axis offset for this layer. Negative = behind, Positive = in front. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layer")
	float DepthOffset = 0.0f;

	/** Editor visibility toggle. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layer")
	bool bVisible = true;

	/** All tile instances on this layer. */
	UPROPERTY(EditAnywhere, Category="Layer")
	TArray<FTileInstance> Tiles;

	/** Find a tile at the given cell. Returns nullptr if empty. */
	const FTileInstance* FindTileAt(FIntPoint Cell) const
	{
		return Tiles.FindByPredicate([Cell](const FTileInstance& T) { return T.CellPosition == Cell; });
	}

	/** Find a tile at the given cell (mutable). */
	FTileInstance* FindTileAtMutable(FIntPoint Cell)
	{
		return Tiles.FindByPredicate([Cell](const FTileInstance& T) { return T.CellPosition == Cell; });
	}

	/** Remove a tile at the given cell. Returns true if something was removed. */
	bool RemoveTileAt(FIntPoint Cell)
	{
		const int32 Removed = Tiles.RemoveAll([Cell](const FTileInstance& T) { return T.CellPosition == Cell; });
		return Removed > 0;
	}
};
