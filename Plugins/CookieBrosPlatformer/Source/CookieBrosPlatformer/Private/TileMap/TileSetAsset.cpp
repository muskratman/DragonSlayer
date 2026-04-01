// Copyright CookieBros. All Rights Reserved.

#include "TileMap/TileSetAsset.h"

const FTileDefinition* UTileSetAsset::FindTileByID(FName TileID) const
{
	return Tiles.FindByPredicate([TileID](const FTileDefinition& Def)
	{
		return Def.TileID == TileID;
	});
}

TArray<FName> UTileSetAsset::GetAllTileIDs() const
{
	TArray<FName> IDs;
	IDs.Reserve(Tiles.Num());
	for (const FTileDefinition& Def : Tiles)
	{
		IDs.Add(Def.TileID);
	}
	return IDs;
}

bool UTileSetAsset::ContainsTileID(FName TileID) const
{
	return FindTileByID(TileID) != nullptr;
}

FPrimaryAssetId UTileSetAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("TileSet"), GetFName());
}

#if WITH_EDITOR
void UTileSetAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Validate: ensure no duplicate TileIDs
	TSet<FName> SeenIDs;
	for (int32 i = 0; i < Tiles.Num(); ++i)
	{
		if (Tiles[i].TileID.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("UTileSetAsset: Tile at index %d has an empty TileID."), i);
			continue;
		}
		if (SeenIDs.Contains(Tiles[i].TileID))
		{
			UE_LOG(LogTemp, Warning, TEXT("UTileSetAsset: Duplicate TileID '%s' at index %d."),
				*Tiles[i].TileID.ToString(), i);
		}
		SeenIDs.Add(Tiles[i].TileID);
	}
}
#endif
