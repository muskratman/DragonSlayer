// Copyright CookieBros. All Rights Reserved.

#include "CookieBrosLevelEditor.h"
#include "EditorModeRegistry.h"
#include "TileEditor/TileEditorMode.h"
#include "TileEditor/TileEditorCommands.h"
#include "TileEditor/TileMapActorDetails.h"
#include "TileMap/TileMapActor.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FCookieBrosLevelEditorModule"

void FCookieBrosLevelEditorModule::StartupModule()
{
	// Register editor commands
	FTileEditorCommands::Register();

	// UEdMode (UTileEditorMode) auto-registers via UCLASS metadata.
	// Info is set in the UTileEditorMode constructor.

	// Register detail customization for ATileMapActor
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		ATileMapActor::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FTileMapActorDetails::MakeInstance)
	);
}

void FCookieBrosLevelEditorModule::ShutdownModule()
{
	// Unregister detail customization
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(ATileMapActor::StaticClass()->GetFName());
	}

	// Unregister commands
	FTileEditorCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCookieBrosLevelEditorModule, CookieBrosLevelEditor)
