// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileMapActor.generated.h"

class UTileMapAsset;
class UTileSetAsset;
class UHierarchicalInstancedStaticMeshComponent;
class UBillboardComponent;

/**
 * ATileMapActor
 * Placed in the level to visualize a UTileMapAsset using HISM components.
 * One HISM component is created per unique mesh per layer for optimal draw-call batching.
 * Special blocks are rendered as billboard icons.
 */
UCLASS(BlueprintType, meta=(DisplayName="Tile Map Actor"))
class COOKIEBROSPLATFORMER_API ATileMapActor : public AActor
{
	GENERATED_BODY()

public:
	ATileMapActor();

	/** The tile map data asset that drives this actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TileMap")
	TObjectPtr<UTileMapAsset> TileMapAsset;

	// ----- Rebuild API -----

	/** Destroy all HISM components and rebuild from TileMapAsset. */
	UFUNCTION(BlueprintCallable, Category="TileMap")
	void RebuildAllInstances();

	/** Rebuild a single layer. */
	UFUNCTION(BlueprintCallable, Category="TileMap")
	void RebuildLayer(int32 LayerIndex);

	// ----- Runtime helpers -----

	/** Get the resolved tile set (loaded from TileMapAsset->TileSet). */
	UFUNCTION(BlueprintPure, Category="TileMap")
	UTileSetAsset* GetTileSet() const;

	/** Get the HISM component for a given mesh key ("LayerIndex_TileID"). Returns nullptr if not found. */
	UHierarchicalInstancedStaticMeshComponent* GetHISMForKey(const FName& Key) const;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	/**
	 * Map of "LayerIdx_TileID" → HISM component.
	 * Dynamically created/destroyed during rebuild. Transient — not saved.
	 */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UHierarchicalInstancedStaticMeshComponent>> HISMComponents;

	/** Root scene component. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> Root;

	/** Internal: clear all HISM components. */
	void ClearAllHISM();

	/** Internal: build instances for one layer given a loaded tile set. */
	void BuildLayerInstances(int32 LayerIndex, const UTileSetAsset* TileSetPtr);

	/** Internal: get or create HISM for a given mesh + layer combo. */
	UHierarchicalInstancedStaticMeshComponent* GetOrCreateHISM(
		const FName& Key,
		UStaticMesh* Mesh,
		bool bEnableCollision);
};
