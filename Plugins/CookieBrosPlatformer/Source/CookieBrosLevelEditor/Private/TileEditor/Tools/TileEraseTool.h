// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "TileEraseTool.generated.h"

class ATileMapActor;

/**
 * UTileEraseToolBuilder
 */
UCLASS()
class UTileEraseToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

/**
 * UTileEraseToolProperties
 */
UCLASS(Transient)
class UTileEraseToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Erase", meta=(ClampMin=0))
	int32 ActiveLayerIndex = 1;
};

/**
 * UTileEraseTool
 * Click on a grid cell to remove the tile at that position.
 */
UCLASS()
class UTileEraseTool : public USingleClickTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;

	UPROPERTY()
	TObjectPtr<UTileEraseToolProperties> EraseProperties;

	void SetTargetActor(ATileMapActor* InActor) { TargetActor = InActor; }

private:
	UPROPERTY()
	TObjectPtr<ATileMapActor> TargetActor;

	bool TraceXZPlane(const FInputDeviceRay& DeviceRay, FVector& OutHitPos) const;
};
