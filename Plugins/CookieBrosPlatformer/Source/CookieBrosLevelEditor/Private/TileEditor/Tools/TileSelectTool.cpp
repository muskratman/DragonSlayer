// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/Tools/TileSelectTool.h"
#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"
#include "InteractiveToolManager.h"
#include "ToolContextInterfaces.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "TileSelectTool"

// ============================================================================
// Builder
// ============================================================================

bool UTileSelectToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UTileSelectToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UTileSelectTool* NewTool = NewObject<UTileSelectTool>(SceneState.ToolManager);
	return NewTool;
}

// ============================================================================
// Tool
// ============================================================================

void UTileSelectTool::Setup()
{
	USingleClickTool::Setup();

	SelectProperties = NewObject<UTileSelectToolProperties>(this);
	AddToolPropertySource(SelectProperties);
}

void UTileSelectTool::Shutdown(EToolShutdownType ShutdownType)
{
	ClearSelection();
	USingleClickTool::Shutdown(ShutdownType);
}

void UTileSelectTool::OnClicked(const FInputDeviceRay& ClickPos)
{
	if (!TargetActor || !TargetActor->TileMapAsset) return;

	FVector HitPos;
	if (!TraceXZPlane(ClickPos, HitPos)) return;

	UTileMapAsset* MapAsset = TargetActor->TileMapAsset;
	const FIntPoint Cell = MapAsset->WorldToCell(HitPos);

	if (!MapAsset->IsCellInBounds(Cell)) return;

	const int32 LayerIdx = SelectProperties->ActiveLayerIndex;
	if (!MapAsset->Layers.IsValidIndex(LayerIdx)) return;

	// Check if Ctrl is held for multi-select (check via modifier key states)
	// Note: In Interactive Tools Framework, modifier keys are typically handled
	// via input behaviors. For simplicity, we toggle selection on re-click.
	const int32 ExistingIdx = SelectedCells.IndexOfByKey(Cell);
	if (ExistingIdx != INDEX_NONE)
	{
		// Deselect on re-click
		SelectedCells.RemoveAt(ExistingIdx);
	}
	else
	{
		SelectedCells.Add(Cell);
	}

	// Update properties with last clicked tile info
	if (const FTileInstance* Tile = MapAsset->GetTileAt(LayerIdx, Cell))
	{
		SelectProperties->SelectedTileID = Tile->TileID;
		SelectProperties->SelectedCell = Tile->CellPosition;
		SelectProperties->SelectedRotation = Tile->RotationStep;
	}
	else
	{
		SelectProperties->SelectedTileID = NAME_None;
		SelectProperties->SelectedCell = Cell;
		SelectProperties->SelectedRotation = 0;
	}
}

void UTileSelectTool::Render(IToolsContextRenderAPI* RenderAPI)
{
	if (!TargetActor || !TargetActor->TileMapAsset) return;
	if (SelectedCells.Num() == 0) return;

	FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
	if (!PDI) return;

	const float CS = TargetActor->TileMapAsset->CellSize;
	float LayerY = 0.0f;
	const int32 LayerIdx = SelectProperties->ActiveLayerIndex;
	if (TargetActor->TileMapAsset->Layers.IsValidIndex(LayerIdx))
	{
		LayerY = TargetActor->TileMapAsset->Layers[LayerIdx].DepthOffset;
	}

	const FLinearColor SelectionColor(0.0f, 1.0f, 0.5f, 1.0f);
	const float Thickness = 2.0f;

	for (const FIntPoint& Cell : SelectedCells)
	{
		// Cell corners in world space (XZ plane at LayerY)
		const float X0 = Cell.X * CS;
		const float X1 = X0 + CS;
		const float Z0 = Cell.Y * CS;
		const float Z1 = Z0 + CS;
		const float Y = LayerY + 1.0f; // Slight offset to avoid Z-fighting

		// Draw rectangle
		PDI->DrawLine(FVector(X0, Y, Z0), FVector(X1, Y, Z0), SelectionColor, SDPG_Foreground, Thickness);
		PDI->DrawLine(FVector(X1, Y, Z0), FVector(X1, Y, Z1), SelectionColor, SDPG_Foreground, Thickness);
		PDI->DrawLine(FVector(X1, Y, Z1), FVector(X0, Y, Z1), SelectionColor, SDPG_Foreground, Thickness);
		PDI->DrawLine(FVector(X0, Y, Z1), FVector(X0, Y, Z0), SelectionColor, SDPG_Foreground, Thickness);
	}
}

void UTileSelectTool::ClearSelection()
{
	SelectedCells.Empty();
	if (SelectProperties)
	{
		SelectProperties->SelectedTileID = NAME_None;
		SelectProperties->SelectedCell = FIntPoint::ZeroValue;
		SelectProperties->SelectedRotation = 0;
	}
}

void UTileSelectTool::DeleteSelectedTiles()
{
	if (!TargetActor || !TargetActor->TileMapAsset) return;
	if (SelectedCells.Num() == 0) return;

	UTileMapAsset* MapAsset = TargetActor->TileMapAsset;
	const int32 LayerIdx = SelectProperties->ActiveLayerIndex;
	if (!MapAsset->Layers.IsValidIndex(LayerIdx)) return;

	FScopedTransaction Transaction(LOCTEXT("DeleteSelectedTiles", "Delete Selected Tiles"));
	MapAsset->Modify();

	for (const FIntPoint& Cell : SelectedCells)
	{
		MapAsset->RemoveTileAt(LayerIdx, Cell);
	}

	TargetActor->RebuildLayer(LayerIdx);
	ClearSelection();
}

bool UTileSelectTool::TraceXZPlane(const FInputDeviceRay& DeviceRay, FVector& OutHitPos) const
{
	float LayerY = 0.0f;
	if (TargetActor && TargetActor->TileMapAsset)
	{
		const int32 LayerIdx = SelectProperties->ActiveLayerIndex;
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
