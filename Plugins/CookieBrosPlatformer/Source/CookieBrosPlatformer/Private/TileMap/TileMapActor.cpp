// Copyright CookieBros. All Rights Reserved.

#include "TileMap/TileMapActor.h"
#include "TileMap/TileMapAsset.h"
#include "TileMap/TileSetAsset.h"
#include "TileMap/TileMapTypes.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

namespace TileMapHelpers
{
	/** Convert RotationStep (0-3) to Yaw degrees. */
	static float RotationStepToYaw(int32 Step)
	{
		return static_cast<float>(FMath::Clamp(Step, 0, 3)) * 90.0f;
	}
}

ATileMapActor::ATileMapActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

// ============================================================================
// Rebuild API
// ============================================================================

void ATileMapActor::RebuildAllInstances()
{
	ClearAllHISM();

	if (!TileMapAsset) return;

	UTileSetAsset* TileSetPtr = GetTileSet();
	if (!TileSetPtr) return;

	for (int32 i = 0; i < TileMapAsset->Layers.Num(); ++i)
	{
		BuildLayerInstances(i, TileSetPtr);
	}
}

void ATileMapActor::RebuildLayer(int32 LayerIndex)
{
	if (!TileMapAsset || !TileMapAsset->Layers.IsValidIndex(LayerIndex)) return;

	UTileSetAsset* TileSetPtr = GetTileSet();
	if (!TileSetPtr) return;

	// Remove existing HISM components for this layer
	const FString Prefix = FString::Printf(TEXT("%d_"), LayerIndex);
	TArray<FName> KeysToRemove;
	for (auto& Pair : HISMComponents)
	{
		if (Pair.Key.ToString().StartsWith(Prefix))
		{
			KeysToRemove.Add(Pair.Key);
		}
	}
	for (const FName& Key : KeysToRemove)
	{
		if (UHierarchicalInstancedStaticMeshComponent* Comp = HISMComponents.FindRef(Key))
		{
			Comp->DestroyComponent();
		}
		HISMComponents.Remove(Key);
	}

	BuildLayerInstances(LayerIndex, TileSetPtr);
}

UTileSetAsset* ATileMapActor::GetTileSet() const
{
	if (!TileMapAsset) return nullptr;
	return TileMapAsset->TileSet.LoadSynchronous();
}

UHierarchicalInstancedStaticMeshComponent* ATileMapActor::GetHISMForKey(const FName& Key) const
{
	const TObjectPtr<UHierarchicalInstancedStaticMeshComponent>* Found = HISMComponents.Find(Key);
	return Found ? Found->Get() : nullptr;
}

// ============================================================================
// Construction / Editor
// ============================================================================

void ATileMapActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RebuildAllInstances();
}

#if WITH_EDITOR
void ATileMapActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropName = PropertyChangedEvent.GetPropertyName();
	if (PropName == GET_MEMBER_NAME_CHECKED(ATileMapActor, TileMapAsset))
	{
		RebuildAllInstances();
	}
}
#endif

// ============================================================================
// Internal
// ============================================================================

void ATileMapActor::ClearAllHISM()
{
	for (auto& Pair : HISMComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	HISMComponents.Empty();
}

void ATileMapActor::BuildLayerInstances(int32 LayerIndex, const UTileSetAsset* TileSetPtr)
{
	check(TileMapAsset);
	check(TileSetPtr);
	check(TileMapAsset->Layers.IsValidIndex(LayerIndex));

	const FTileMapLayer& Layer = TileMapAsset->Layers[LayerIndex];
	if (!Layer.bVisible) return;

	const float DepthY = Layer.DepthOffset;
	const float CS = TileMapAsset->CellSize;

	for (const FTileInstance& Tile : Layer.Tiles)
	{
		const FTileDefinition* Def = TileSetPtr->FindTileByID(Tile.TileID);
		if (!Def) continue;

		// Special blocks don't get HISM instances (rendered as editor-only billboards)
		if (Def->bIsSpecialBlock) continue;

		UStaticMesh* Mesh = Def->Mesh.LoadSynchronous();
		if (!Mesh) continue;

		// Key format: "LayerIndex_TileID"
		const FName Key = FName(*FString::Printf(TEXT("%d_%s"), LayerIndex, *Def->TileID.ToString()));

		UHierarchicalInstancedStaticMeshComponent* HISM =
			GetOrCreateHISM(Key, Mesh, Def->bCollisionEnabled);

		// Compute world transform
		const FVector WorldPos = TileMapAsset->CellToWorld(Tile.CellPosition, DepthY);

		// Rotation around Y axis (depth axis in side-view)
		const float Yaw = TileMapHelpers::RotationStepToYaw(Tile.RotationStep);
		const FRotator Rot(0.0f, Yaw, 0.0f);

		// Scale mesh to fit the cell footprint (SizeInCells * CellSize).
		// Source meshes are assumed to be unit-scale (100×100×100 UU = 1m cube).
		const FVector Scale(
			static_cast<float>(Def->SizeInCells.X) * CS / 100.0f,
			1.0f, // Y axis (depth) — keep 1:1
			static_cast<float>(Def->SizeInCells.Y) * CS / 100.0f
		);

		const FTransform InstanceTransform(Rot.Quaternion(), WorldPos, Scale);
		HISM->AddInstance(InstanceTransform, /*bWorldSpace=*/ false);
	}

	// Batch-build trees for all affected HISM components this layer
	const FString Prefix = FString::Printf(TEXT("%d_"), LayerIndex);
	for (auto& Pair : HISMComponents)
	{
		if (Pair.Key.ToString().StartsWith(Prefix) && Pair.Value)
		{
			Pair.Value->BuildTreeIfOutdated(true, false);
		}
	}
}

UHierarchicalInstancedStaticMeshComponent* ATileMapActor::GetOrCreateHISM(
	const FName& Key,
	UStaticMesh* Mesh,
	bool bEnableCollision)
{
	if (UHierarchicalInstancedStaticMeshComponent* Existing = GetHISMForKey(Key))
	{
		return Existing;
	}

	UHierarchicalInstancedStaticMeshComponent* NewHISM =
		NewObject<UHierarchicalInstancedStaticMeshComponent>(this, Key);
	NewHISM->SetStaticMesh(Mesh);
	NewHISM->SetMobility(EComponentMobility::Static);
	NewHISM->SetCollisionEnabled(bEnableCollision
		? ECollisionEnabled::QueryAndPhysics
		: ECollisionEnabled::NoCollision);
	NewHISM->SetGenerateOverlapEvents(false);
	NewHISM->NumCustomDataFloats = 0;
	NewHISM->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	NewHISM->RegisterComponent();

	HISMComponents.Add(Key, NewHISM);
	return NewHISM;
}
