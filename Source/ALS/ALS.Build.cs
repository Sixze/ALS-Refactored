using UnrealBuildTool;

public class ALS : ModuleRules
{
	public ALS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "NetCore", "Engine", "PhysicsCore", "GameplayTags", "AnimGraph", "ControlRig", "Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"BlueprintGraph", "RigVM"
		});
	}
}