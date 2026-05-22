// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CommonPools : ModuleRules
{
	public CommonPools(ReadOnlyTargetRules Target) : base(Target)
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
		]);

		PrivateDependencyModuleNames.AddRange([
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"GameplayTags",
			// ... add private dependencies that you statically link with here ...	
		]);

		DynamicallyLoadedModuleNames.AddRange([
			// ... add any modules that your module loads dynamically here ...
		]);
	}
}
