/*
 SimplexNoise 1.2.0
 -----
 DevDad - Afan Olovcic @ www.art-and-code.com - 08/12/2015
*/
using UnrealBuildTool;

public class SimplexNoise : ModuleRules
{
    public SimplexNoise(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        //Private Paths
        PrivateIncludePaths.AddRange(new string[] {
            "SimplexNoise/Private"
        });

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine"
            }
        );
    }
}
