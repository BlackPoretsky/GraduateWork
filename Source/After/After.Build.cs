// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class After : ModuleRules
{
    public After(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "After"
            }
        );

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "EnhancedInput",
            "ModularGameplay",
            "ReplicationGraph",
            "GameFeatures",
            "Niagara"});

        /* Gamplay Ability System*/
        PublicDependencyModuleNames.AddRange(new string[] {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"});

        /* Game Classses */
        PublicIncludePaths.AddRange(new string[] {
            "After/Public/Inventory",
            "After/Public/Item",
            "After/Public/Camera",
            "After/Public/AbilitySystem",
            "After/Public/Input",
            "After/Public/Component",
            "After/Public/Character",
            "After/Public/Item/Weapon",
            "After/Public/Animation",
            "After/Public/System"});

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "NetCore",
            "GameplayMessageRuntime"
        });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        SetupIrisSupport(Target);
    }
}
