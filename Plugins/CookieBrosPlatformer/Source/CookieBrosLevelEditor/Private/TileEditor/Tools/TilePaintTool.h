// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "TilePaintTool.generated.h"

class ATileMapActor;
class UTileSetAsset;

/**
 * UTilePaintToolBuilder
 * Creates UTilePaintTool instances when the Paint tool is activated.
 */
UCLASS()
class UTilePaintToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

/**
 * UTilePaintToolProperties
 * Editable properties shown in the mode details panel while painting.
 */
UCLASS(Transient)
class UTilePaintToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	/** The tile ID currently selected for painting. Set by the palette widget. */
	UPROPERTY(EditAnywhere, Category="Paint")
	FName SelectedTileID;

	/** Current rotation step (0-3). */
	UPROPERTY(EditAnywhere, Category="Paint", meta=(ClampMin=0, ClampMax=3))
	int32 RotationStep = 0;

	/** Active layer index. */
	UPROPERTY(EditAnywhere, Category="Paint", meta=(ClampMin=0))
	int32 ActiveLayerIndex = 1; // Default = Ground
};

/**
 * UTilePaintTool
 * Interactive tool for placing tiles onto the grid.
 * Uses SingleClickTool behavior: each click places a tile at the cursor's grid cell.
 */
UCLASS()
class UTilePaintTool : public USingleClickTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	/** Handle a click — place tile at the XZ grid cell under the cursor. */
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;

	/** Access the paint properties. */
	UPROPERTY()
	TObjectPtr<UTilePaintToolProperties> PaintProperties;

	/** Set the target tile map actor. */
	void SetTargetActor(ATileMapActor* InActor) { TargetActor = InActor; }

private:
	/** The actor we are painting onto. */
	UPROPERTY()
	TObjectPtr<ATileMapActor> TargetActor;

	/** Trace XZ plane at cursor and return world position. */
	bool TraceXZPlane(const FInputDeviceRay& DeviceRay, FVector& OutHitPos) const;
};
