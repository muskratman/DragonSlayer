// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "TileMap/TileMapTypes.h"
#include "TileSelectTool.generated.h"

class ATileMapActor;

/**
 * UTileSelectToolBuilder
 */
UCLASS()
class UTileSelectToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

/**
 * UTileSelectToolProperties
 */
UCLASS(Transient)
class UTileSelectToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Select", meta=(ClampMin=0))
	int32 ActiveLayerIndex = 1;

	/** Read-only: info about the last selected tile. */
	UPROPERTY(VisibleAnywhere, Category="Select|Info")
	FName SelectedTileID;

	UPROPERTY(VisibleAnywhere, Category="Select|Info")
	FIntPoint SelectedCell = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, Category="Select|Info")
	int32 SelectedRotation = 0;
};

/**
 * UTileSelectTool
 * Click on a grid cell to select the tile and display its properties.
 * Supports batch selection via Ctrl+Click (adds to current selection).
 * Selected cells are highlighted in the viewport via Render().
 */
UCLASS()
class UTileSelectTool : public USingleClickTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

	UPROPERTY()
	TObjectPtr<UTileSelectToolProperties> SelectProperties;

	void SetTargetActor(ATileMapActor* InActor) { TargetActor = InActor; }

	/** Get all currently selected cells. */
	const TArray<FIntPoint>& GetSelectedCells() const { return SelectedCells; }

	/** Clear the selection. */
	void ClearSelection();

	/** Delete all selected tiles (with undo). */
	void DeleteSelectedTiles();

private:
	UPROPERTY()
	TObjectPtr<ATileMapActor> TargetActor;

	/** Currently selected cells. */
	TArray<FIntPoint> SelectedCells;

	bool TraceXZPlane(const FInputDeviceRay& DeviceRay, FVector& OutHitPos) const;
};
