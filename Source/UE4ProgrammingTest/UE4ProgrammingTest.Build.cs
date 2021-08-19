// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE4ProgrammingTest : ModuleRules
{
	public UE4ProgrammingTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities", "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule" });
    }
}
