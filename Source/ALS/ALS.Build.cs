using UnrealBuildTool;

public class ALS : ModuleRules
{
	public ALS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "NetCore", "PhysicsCore", "GameplayTags", "AnimGraphRuntime", "ControlRig", "RigVM", "Niagara"
		});
	}
}