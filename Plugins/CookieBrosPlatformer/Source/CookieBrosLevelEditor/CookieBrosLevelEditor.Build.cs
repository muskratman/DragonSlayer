// Copyright CookieBros. All Rights Reserved.

using UnrealBuildTool;

public class CookieBrosLevelEditor : ModuleRules
{
	public CookieBrosLevelEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"CookieBrosPlatformer"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"UnrealEd",
				"EditorFramework",
				"LevelEditor",
				"PropertyEditor",
				"InteractiveToolsFramework",
				"EditorInteractiveToolsFramework",
				"ToolMenus",
				"Projects",
				"Json",
				"JsonUtilities",
				"GameplayTags"
			}
		);
	}
}
