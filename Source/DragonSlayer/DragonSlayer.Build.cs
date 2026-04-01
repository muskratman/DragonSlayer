// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DragonSlayer : ModuleRules
{
	public DragonSlayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"CookieBrosPlatformer",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Niagara",
			"SlateCore",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"DragonSlayer",
			"DragonSlayer/Platformer/Base",
			"DragonSlayer/Platformer/Camera",
			"DragonSlayer/Platformer/Character",
			"DragonSlayer/Platformer/Environment",
			"DragonSlayer/UI",
			"DragonSlayer/UI/PauseMenu",
			"DragonSlayer/Core/UI",
			"DragonSlayer/Core/UI/MainMenu"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
