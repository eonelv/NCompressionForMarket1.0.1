// Copyright 2024 ngcod, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NCompression : ModuleRules
{
	public NCompression(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"zlib"
			}
			);
	}
}
