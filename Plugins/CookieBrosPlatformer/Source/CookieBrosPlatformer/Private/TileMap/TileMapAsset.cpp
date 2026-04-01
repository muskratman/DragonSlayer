// Copyright CookieBros. All Rights Reserved.

#include "TileMap/TileMapAsset.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

// ============================================================================
// Tile CRUD
// ============================================================================

void UTileMapAsset::AddTile(int32 LayerIndex, const FTileInstance& Tile)
{
	if (!Layers.IsValidIndex(LayerIndex)) return;

	FTileMapLayer& Layer = Layers[LayerIndex];

	// Overwrite existing tile at the same cell
	Layer.RemoveTileAt(Tile.CellPosition);
	Layer.Tiles.Add(Tile);
}

bool UTileMapAsset::RemoveTileAt(int32 LayerIndex, FIntPoint Cell)
{
	if (!Layers.IsValidIndex(LayerIndex)) return false;
	return Layers[LayerIndex].RemoveTileAt(Cell);
}

const FTileInstance* UTileMapAsset::GetTileAt(int32 LayerIndex, FIntPoint Cell) const
{
	if (!Layers.IsValidIndex(LayerIndex)) return nullptr;
	return Layers[LayerIndex].FindTileAt(Cell);
}

bool UTileMapAsset::HasTileAt(int32 LayerIndex, FIntPoint Cell) const
{
	return GetTileAt(LayerIndex, Cell) != nullptr;
}

void UTileMapAsset::ClearLayer(int32 LayerIndex)
{
	if (!Layers.IsValidIndex(LayerIndex)) return;
	Layers[LayerIndex].Tiles.Empty();
}

// ============================================================================
// Coordinate helpers
// ============================================================================

FIntPoint UTileMapAsset::WorldToCell(const FVector& WorldPos) const
{
	// X → column, Z → row. Y is depth (ignored for cell computation).
	const int32 Col = FMath::FloorToInt32(WorldPos.X / CellSize);
	const int32 Row = FMath::FloorToInt32(WorldPos.Z / CellSize);
	return FIntPoint(Col, Row);
}

FVector UTileMapAsset::CellToWorld(FIntPoint Cell, float LayerDepthY) const
{
	// Center of the cell
	const float X = (static_cast<float>(Cell.X) + 0.5f) * CellSize;
	const float Z = (static_cast<float>(Cell.Y) + 0.5f) * CellSize;
	return FVector(X, LayerDepthY, Z);
}

bool UTileMapAsset::IsCellInBounds(FIntPoint Cell) const
{
	return Cell.X >= 0 && Cell.X < MapSizeInCells.X
		&& Cell.Y >= 0 && Cell.Y < MapSizeInCells.Y;
}

// ============================================================================
// JSON Export / Import
// ============================================================================

void UTileMapAsset::ExportToJson(const FString& FilePath) const
{
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("MapName"), MapName.ToString());
	Root->SetNumberField(TEXT("CellSize"), CellSize);

	// MapSize
	TSharedRef<FJsonObject> SizeObj = MakeShared<FJsonObject>();
	SizeObj->SetNumberField(TEXT("X"), MapSizeInCells.X);
	SizeObj->SetNumberField(TEXT("Y"), MapSizeInCells.Y);
	Root->SetObjectField(TEXT("MapSizeInCells"), SizeObj);

	// Layers
	TArray<TSharedPtr<FJsonValue>> LayersArr;
	for (const FTileMapLayer& Layer : Layers)
	{
		TSharedRef<FJsonObject> LayerObj = MakeShared<FJsonObject>();
		LayerObj->SetStringField(TEXT("LayerName"), Layer.LayerName.ToString());
		LayerObj->SetNumberField(TEXT("DepthOffset"), Layer.DepthOffset);
		LayerObj->SetBoolField(TEXT("bVisible"), Layer.bVisible);

		TArray<TSharedPtr<FJsonValue>> TilesArr;
		for (const FTileInstance& Tile : Layer.Tiles)
		{
			TSharedRef<FJsonObject> TileObj = MakeShared<FJsonObject>();
			TileObj->SetStringField(TEXT("TileID"), Tile.TileID.ToString());
			TileObj->SetNumberField(TEXT("CellX"), Tile.CellPosition.X);
			TileObj->SetNumberField(TEXT("CellY"), Tile.CellPosition.Y);
			TileObj->SetNumberField(TEXT("Rotation"), Tile.RotationStep);
			TilesArr.Add(MakeShared<FJsonValueObject>(TileObj));
		}
		LayerObj->SetArrayField(TEXT("Tiles"), TilesArr);
		LayersArr.Add(MakeShared<FJsonValueObject>(LayerObj));
	}
	Root->SetArrayField(TEXT("Layers"), LayersArr);

	FString Output;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(Root, Writer);
	FFileHelper::SaveStringToFile(Output, *FilePath);
}

bool UTileMapAsset::ImportFromJson(const FString& FilePath)
{
	FString Input;
	if (!FFileHelper::LoadFileToString(Input, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("UTileMapAsset::ImportFromJson: Failed to load file '%s'."), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UTileMapAsset::ImportFromJson: Failed to parse JSON."));
		return false;
	}

	MapName = FName(*Root->GetStringField(TEXT("MapName")));
	CellSize = Root->GetNumberField(TEXT("CellSize"));

	const TSharedPtr<FJsonObject>* SizeObj = nullptr;
	if (Root->TryGetObjectField(TEXT("MapSizeInCells"), SizeObj))
	{
		MapSizeInCells.X = static_cast<int32>((*SizeObj)->GetNumberField(TEXT("X")));
		MapSizeInCells.Y = static_cast<int32>((*SizeObj)->GetNumberField(TEXT("Y")));
	}

	Layers.Empty();
	const TArray<TSharedPtr<FJsonValue>>* LayersArr = nullptr;
	if (Root->TryGetArrayField(TEXT("Layers"), LayersArr))
	{
		for (const TSharedPtr<FJsonValue>& LayerVal : *LayersArr)
		{
			const TSharedPtr<FJsonObject>& LayerObj = LayerVal->AsObject();
			FTileMapLayer Layer;
			Layer.LayerName = FName(*LayerObj->GetStringField(TEXT("LayerName")));
			Layer.DepthOffset = LayerObj->GetNumberField(TEXT("DepthOffset"));
			Layer.bVisible = LayerObj->GetBoolField(TEXT("bVisible"));

			const TArray<TSharedPtr<FJsonValue>>* TilesArr = nullptr;
			if (LayerObj->TryGetArrayField(TEXT("Tiles"), TilesArr))
			{
				for (const TSharedPtr<FJsonValue>& TileVal : *TilesArr)
				{
					const TSharedPtr<FJsonObject>& TileObj = TileVal->AsObject();
					FTileInstance Tile;
					Tile.TileID = FName(*TileObj->GetStringField(TEXT("TileID")));
					Tile.CellPosition.X = static_cast<int32>(TileObj->GetNumberField(TEXT("CellX")));
					Tile.CellPosition.Y = static_cast<int32>(TileObj->GetNumberField(TEXT("CellY")));
					Tile.RotationStep = static_cast<int32>(TileObj->GetNumberField(TEXT("Rotation")));
					Layer.Tiles.Add(Tile);
				}
			}
			Layers.Add(Layer);
		}
	}

	return true;
}

// ============================================================================
// UPrimaryDataAsset
// ============================================================================

FPrimaryAssetId UTileMapAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("TileMap"), GetFName());
}

// ============================================================================
// Default layers
// ============================================================================

void UTileMapAsset::EnsureDefaultLayers()
{
	if (Layers.Num() > 0) return;

	auto MakeLayer = [](FName Name, float Depth) -> FTileMapLayer
	{
		FTileMapLayer L;
		L.LayerName = Name;
		L.DepthOffset = Depth;
		L.bVisible = true;
		return L;
	};

	Layers.Add(MakeLayer(FName("Background"), -200.0f));
	Layers.Add(MakeLayer(FName("Ground"), 0.0f));
	Layers.Add(MakeLayer(FName("Foreground"), 200.0f));
	Layers.Add(MakeLayer(FName("Special"), 0.0f));
}
