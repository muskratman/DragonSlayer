// Copyright CookieBros. All Rights Reserved.

#include "TileEditor/TileEditorMode.h"
#include "TileEditor/TileEditorModeToolkit.h"
#include "TileEditor/TileEditorCommands.h"
#include "TileEditor/Tools/TilePaintTool.h"
#include "TileEditor/Tools/TileEraseTool.h"
#include "TileEditor/Tools/TileSelectTool.h"
#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"
#include "TileMap/TileSetAsset.h"
#include "InteractiveToolManager.h"
#include "EdModeInteractiveToolsContext.h"
#include "EngineUtils.h"
#include "Editor.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "TileEditorMode"

const FEditorModeID UTileEditorMode::EM_TileEditorModeId = TEXT("EM_TileEditor_2_5D");

const FString UTileEditorMode::ToolName_Paint  = TEXT("TileEditor_Paint");
const FString UTileEditorMode::ToolName_Erase  = TEXT("TileEditor_Erase");
const FString UTileEditorMode::ToolName_Select = TEXT("TileEditor_Select");

UTileEditorMode::UTileEditorMode()
{
	Info = FEditorModeInfo(
		EM_TileEditorModeId,
		LOCTEXT("ModeName", "Tile Editor 2.5D"),
		FSlateIcon(),
		true,  // bVisible
		500    // PriorityOrder
	);
}

// ============================================================================
// Enter / Exit
// ============================================================================

void UTileEditorMode::Enter()
{
	UEdMode::Enter();

	// Auto-find a TileMapActor in the level
	ActiveTileMapActor = FindTileMapActorInLevel();

	// Register tool builders
	UInteractiveToolManager* ToolManager = GetToolManager();
	if (ToolManager)
	{
		ToolManager->RegisterToolType(ToolName_Paint, NewObject<UTilePaintToolBuilder>(this));
		ToolManager->RegisterToolType(ToolName_Erase, NewObject<UTileEraseToolBuilder>(this));
		ToolManager->RegisterToolType(ToolName_Select, NewObject<UTileSelectToolBuilder>(this));
	}

	// Bind hotkeys
	BindCommands();

	// Activate paint tool by default
	if (ToolManager)
	{
		ToolManager->SelectActiveToolType(EToolSide::Left, ToolName_Paint);
	}
}

void UTileEditorMode::Exit()
{
	UInteractiveToolManager* ToolManager = GetToolManager();
	if (ToolManager)
	{
		ToolManager->UnregisterToolType(ToolName_Paint);
		ToolManager->UnregisterToolType(ToolName_Erase);
		ToolManager->UnregisterToolType(ToolName_Select);
	}

	ActiveTileMapActor = nullptr;

	UEdMode::Exit();
}

// ============================================================================
// Toolkit
// ============================================================================

void UTileEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FTileEditorModeToolkit);
}

// NOTE: UEdMode doesn't have Tick/Render overrides.
// Grid overlay and ghost preview will be implemented via tool Render() methods.

// ============================================================================
// Selection filtering
// ============================================================================

bool UTileEditorMode::IsSelectionAllowed(AActor* InActor, bool bInSelection) const
{
	// Only allow selecting TileMapActors while in tile editor mode
	return InActor && InActor->IsA<ATileMapActor>();
}

// ============================================================================
// Commands
// ============================================================================

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UTileEditorMode::GetModeCommands() const
{
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;

	const FTileEditorCommands& Cmds = FTileEditorCommands::Get();

	TArray<TSharedPtr<FUICommandInfo>> ToolCommands;
	ToolCommands.Add(Cmds.ActivatePaintTool);
	ToolCommands.Add(Cmds.ActivateEraseTool);
	ToolCommands.Add(Cmds.ActivateSelectTool);
	Commands.Add(FName("Tools"), ToolCommands);

	TArray<TSharedPtr<FUICommandInfo>> EditCommands;
	EditCommands.Add(Cmds.RotateTileCW);
	EditCommands.Add(Cmds.RotateTileCCW);
	Commands.Add(FName("Edit"), EditCommands);

	TArray<TSharedPtr<FUICommandInfo>> LayerCommands;
	LayerCommands.Add(Cmds.SelectLayer1);
	LayerCommands.Add(Cmds.SelectLayer2);
	LayerCommands.Add(Cmds.SelectLayer3);
	LayerCommands.Add(Cmds.SelectLayer4);
	Commands.Add(FName("Layers"), LayerCommands);

	return Commands;
}

// ============================================================================
// State management
// ============================================================================

void UTileEditorMode::RotateCW()
{
	CurrentRotationStep = (CurrentRotationStep + 1) % 4;
}

void UTileEditorMode::RotateCCW()
{
	CurrentRotationStep = (CurrentRotationStep + 3) % 4; // +3 ≡ -1 mod 4
}

void UTileEditorMode::SetActiveLayer(int32 LayerIndex)
{
	ActiveLayerIndex = LayerIndex;

	// Propagate to active tool properties
	UInteractiveToolManager* ToolManager = GetToolManager();
	if (!ToolManager) return;

	UInteractiveTool* ActiveTool = ToolManager->GetActiveTool(EToolSide::Left);
	if (!ActiveTool) return;

	if (UTilePaintTool* PaintTool = Cast<UTilePaintTool>(ActiveTool))
	{
		PaintTool->PaintProperties->ActiveLayerIndex = LayerIndex;
	}
	else if (UTileEraseTool* EraseTool = Cast<UTileEraseTool>(ActiveTool))
	{
		EraseTool->EraseProperties->ActiveLayerIndex = LayerIndex;
	}
	else if (UTileSelectTool* SelectTool = Cast<UTileSelectTool>(ActiveTool))
	{
		SelectTool->SelectProperties->ActiveLayerIndex = LayerIndex;
	}
}

void UTileEditorMode::OnPaletteTileSelected(FName TileID)
{
	SelectedTileID = TileID;

	// Propagate to paint tool
	UInteractiveToolManager* ToolManager = GetToolManager();
	if (!ToolManager) return;

	UInteractiveTool* ActiveTool = ToolManager->GetActiveTool(EToolSide::Left);
	if (UTilePaintTool* PaintTool = Cast<UTilePaintTool>(ActiveTool))
	{
		PaintTool->PaintProperties->SelectedTileID = TileID;
	}
}

// ============================================================================
// Internal
// ============================================================================

ATileMapActor* UTileEditorMode::FindTileMapActorInLevel() const
{
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World) return nullptr;

	for (TActorIterator<ATileMapActor> It(World); It; ++It)
	{
		return *It; // Return first found
	}
	return nullptr;
}

void UTileEditorMode::BindCommands()
{
	const FTileEditorCommands& Cmds = FTileEditorCommands::Get();
	const TSharedRef<FUICommandList>& CommandList = Toolkit->GetToolkitCommands();

	// Tool activation
	CommandList->MapAction(Cmds.ActivatePaintTool,
		FExecuteAction::CreateLambda([this]()
		{
			if (UInteractiveToolManager* TM = GetToolManager())
				TM->SelectActiveToolType(EToolSide::Left, ToolName_Paint);
		}));

	CommandList->MapAction(Cmds.ActivateEraseTool,
		FExecuteAction::CreateLambda([this]()
		{
			if (UInteractiveToolManager* TM = GetToolManager())
				TM->SelectActiveToolType(EToolSide::Left, ToolName_Erase);
		}));

	CommandList->MapAction(Cmds.ActivateSelectTool,
		FExecuteAction::CreateLambda([this]()
		{
			if (UInteractiveToolManager* TM = GetToolManager())
				TM->SelectActiveToolType(EToolSide::Left, ToolName_Select);
		}));

	// Rotation
	CommandList->MapAction(Cmds.RotateTileCW,
		FExecuteAction::CreateUObject(this, &UTileEditorMode::RotateCW));

	CommandList->MapAction(Cmds.RotateTileCCW,
		FExecuteAction::CreateUObject(this, &UTileEditorMode::RotateCCW));

	// Layer switching
	CommandList->MapAction(Cmds.SelectLayer1,
		FExecuteAction::CreateLambda([this]() { SetActiveLayer(0); }));
	CommandList->MapAction(Cmds.SelectLayer2,
		FExecuteAction::CreateLambda([this]() { SetActiveLayer(1); }));
	CommandList->MapAction(Cmds.SelectLayer3,
		FExecuteAction::CreateLambda([this]() { SetActiveLayer(2); }));
	CommandList->MapAction(Cmds.SelectLayer4,
		FExecuteAction::CreateLambda([this]() { SetActiveLayer(3); }));
}

#undef LOCTEXT_NAMESPACE
