// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/Tools/TileEraseTool.h"
#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"
#include "InteractiveToolManager.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "TileEraseTool"

// ============================================================================
// Builder
// ============================================================================

bool UTileEraseToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UTileEraseToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UTileEraseTool* NewTool = NewObject<UTileEraseTool>(SceneState.ToolManager);
	return NewTool;
}

// ============================================================================
// Tool
// ============================================================================

void UTileEraseTool::Setup()
{
	USingleClickTool::Setup();

	EraseProperties = NewObject<UTileEraseToolProperties>(this);
	AddToolPropertySource(EraseProperties);
}

void UTileEraseTool::Shutdown(EToolShutdownType ShutdownType)
{
	USingleClickTool::Shutdown(ShutdownType);
}

void UTileEraseTool::OnClicked(const FInputDeviceRay& ClickPos)
{
	if (!TargetActor || !TargetActor->TileMapAsset) return;

	FVector HitPos;
	if (!TraceXZPlane(ClickPos, HitPos)) return;

	UTileMapAsset* MapAsset = TargetActor->TileMapAsset;
	const FIntPoint Cell = MapAsset->WorldToCell(HitPos);

	if (!MapAsset->IsCellInBounds(Cell)) return;

	const int32 LayerIdx = EraseProperties->ActiveLayerIndex;
	if (!MapAsset->Layers.IsValidIndex(LayerIdx)) return;

	if (!MapAsset->HasTileAt(LayerIdx, Cell)) return;

	FScopedTransaction Transaction(LOCTEXT("EraseTile", "Erase Tile"));
	MapAsset->Modify();

	MapAsset->RemoveTileAt(LayerIdx, Cell);
	TargetActor->RebuildLayer(LayerIdx);
}

bool UTileEraseTool::TraceXZPlane(const FInputDeviceRay& DeviceRay, FVector& OutHitPos) const
{
	float LayerY = 0.0f;
	if (TargetActor && TargetActor->TileMapAsset)
	{
		const int32 LayerIdx = EraseProperties->ActiveLayerIndex;
		if (TargetActor->TileMapAsset->Layers.IsValidIndex(LayerIdx))
		{
			LayerY = TargetActor->TileMapAsset->Layers[LayerIdx].DepthOffset;
		}
	}

	const FVector& Origin = DeviceRay.WorldRay.Origin;
	const FVector& Dir = DeviceRay.WorldRay.Direction;

	if (FMath::IsNearlyZero(Dir.Y)) return false;

	const float T = (LayerY - Origin.Y) / Dir.Y;
	if (T < 0.0f) return false;

	OutHitPos = Origin + Dir * T;
	return true;
}

#undef LOCTEXT_NAMESPACE
