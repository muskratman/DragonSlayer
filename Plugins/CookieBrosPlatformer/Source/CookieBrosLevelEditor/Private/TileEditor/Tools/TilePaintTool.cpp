// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/Tools/TilePaintTool.h"
#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"
#include "TileMap/TileSetAsset.h"
#include "TileMap/TileMapTypes.h"
#include "InteractiveToolManager.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "TilePaintTool"

// ============================================================================
// Builder
// ============================================================================

bool UTilePaintToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true; // Always available when the mode is active
}

UInteractiveTool* UTilePaintToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UTilePaintTool* NewTool = NewObject<UTilePaintTool>(SceneState.ToolManager);
	return NewTool;
}

// ============================================================================
// Tool
// ============================================================================

void UTilePaintTool::Setup()
{
	USingleClickTool::Setup();

	PaintProperties = NewObject<UTilePaintToolProperties>(this);
	AddToolPropertySource(PaintProperties);
}

void UTilePaintTool::Shutdown(EToolShutdownType ShutdownType)
{
	USingleClickTool::Shutdown(ShutdownType);
}

void UTilePaintTool::OnClicked(const FInputDeviceRay& ClickPos)
{
	if (!TargetActor || !TargetActor->TileMapAsset) return;
	if (PaintProperties->SelectedTileID.IsNone()) return;

	FVector HitPos;
	if (!TraceXZPlane(ClickPos, HitPos)) return;

	UTileMapAsset* MapAsset = TargetActor->TileMapAsset;
	const FIntPoint Cell = MapAsset->WorldToCell(HitPos);

	if (!MapAsset->IsCellInBounds(Cell)) return;

	const int32 LayerIdx = PaintProperties->ActiveLayerIndex;
	if (!MapAsset->Layers.IsValidIndex(LayerIdx)) return;

	// Construct tile instance
	FTileInstance NewTile;
	NewTile.TileID = PaintProperties->SelectedTileID;
	NewTile.CellPosition = Cell;
	NewTile.RotationStep = PaintProperties->RotationStep;

	// Undo support
	FScopedTransaction Transaction(LOCTEXT("PlaceTile", "Place Tile"));
	MapAsset->Modify();

	MapAsset->AddTile(LayerIdx, NewTile);
	TargetActor->RebuildLayer(LayerIdx);
}

bool UTilePaintTool::TraceXZPlane(const FInputDeviceRay& DeviceRay, FVector& OutHitPos) const
{
	// Intersect ray with XZ plane at Y = layer depth
	float LayerY = 0.0f;
	if (TargetActor && TargetActor->TileMapAsset)
	{
		const int32 LayerIdx = PaintProperties->ActiveLayerIndex;
		if (TargetActor->TileMapAsset->Layers.IsValidIndex(LayerIdx))
		{
			LayerY = TargetActor->TileMapAsset->Layers[LayerIdx].DepthOffset;
		}
	}

	const FVector& Origin = DeviceRay.WorldRay.Origin;
	const FVector& Dir = DeviceRay.WorldRay.Direction;

	// Plane: Y = LayerY → solve t: Origin.Y + t * Dir.Y = LayerY
	if (FMath::IsNearlyZero(Dir.Y))
	{
		// Ray is parallel to the plane
		OutHitPos = FVector::ZeroVector;
		return false;
	}

	const float T = (LayerY - Origin.Y) / Dir.Y;
	if (T < 0.0f) return false; // Behind the camera

	OutHitPos = Origin + Dir * T;
	return true;
}

#undef LOCTEXT_NAMESPACE
