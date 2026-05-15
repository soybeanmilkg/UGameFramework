// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameFeatureExtension : ModuleRules
{
	public GameFeatureExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange([
			// ... add public include paths required here ...
		]);

		PrivateIncludePaths.AddRange([
			// ... add other private include paths required here ...
		]);

		PublicDependencyModuleNames.AddRange([
			"Core",
			// ... add other public dependencies that you statically link with here ...
			"GameFeatures",
			"GameplayAbilities",
			"GameplayTags",
			"CommonUI",
			
			"UMG",
			"CommonGame",
			"UIExtension",
			"ModularGameplay",
			"EnhancedInput",
		]);

		PrivateDependencyModuleNames.AddRange([
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			// ... add private dependencies that you statically link with here ...	
		]);

		DynamicallyLoadedModuleNames.AddRange([
			// ... add any modules that your module loads dynamically here ...
		]);
	}
}
