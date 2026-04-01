// Copyright CookieBros. All Rights Reserved.

#include "TileMap/CBPTileMapBlueprintLibrary.h"
#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"

bool UCBPTileMapLibrary::AddTile(ATileMapActor* Actor, int32 LayerIndex,
                                       FIntPoint Cell, FName TileID, int32 RotationStep)
{
	if (!Actor || !Actor->TileMapAsset) return false;

	UTileMapAsset* Map = Actor->TileMapAsset;
	if (!Map->Layers.IsValidIndex(LayerIndex)) return false;
	if (!Map->IsCellInBounds(Cell)) return false;

	FTileInstance NewTile;
	NewTile.TileID = TileID;
	NewTile.CellPosition = Cell;
	NewTile.RotationStep = FMath::Clamp(RotationStep, 0, 3);

	Map->AddTile(LayerIndex, NewTile);
	Actor->RebuildLayer(LayerIndex);
	return true;
}

bool UCBPTileMapLibrary::RemoveTile(ATileMapActor* Actor, int32 LayerIndex, FIntPoint Cell)
{
	if (!Actor || !Actor->TileMapAsset) return false;

	UTileMapAsset* Map = Actor->TileMapAsset;
	if (!Map->RemoveTileAt(LayerIndex, Cell)) return false;

	Actor->RebuildLayer(LayerIndex);
	return true;
}

FTileInstance UCBPTileMapLibrary::GetTileAt(ATileMapActor* Actor, int32 LayerIndex, FIntPoint Cell)
{
	if (!Actor || !Actor->TileMapAsset) return FTileInstance();

	const FTileInstance* Found = Actor->TileMapAsset->GetTileAt(LayerIndex, Cell);
	return Found ? *Found : FTileInstance();
}

bool UCBPTileMapLibrary::HasTileAt(ATileMapActor* Actor, int32 LayerIndex, FIntPoint Cell)
{
	if (!Actor || !Actor->TileMapAsset) return false;
	return Actor->TileMapAsset->HasTileAt(LayerIndex, Cell);
}

void UCBPTileMapLibrary::ClearLayer(ATileMapActor* Actor, int32 LayerIndex)
{
	if (!Actor || !Actor->TileMapAsset) return;

	Actor->TileMapAsset->ClearLayer(LayerIndex);
	Actor->RebuildLayer(LayerIndex);
}

FIntPoint UCBPTileMapLibrary::GetMapSize(ATileMapActor* Actor)
{
	if (!Actor || !Actor->TileMapAsset) return FIntPoint::ZeroValue;
	return Actor->TileMapAsset->MapSizeInCells;
}

FIntPoint UCBPTileMapLibrary::WorldToCell(ATileMapActor* Actor, FVector WorldPos)
{
	if (!Actor || !Actor->TileMapAsset) return FIntPoint::ZeroValue;
	return Actor->TileMapAsset->WorldToCell(WorldPos);
}
