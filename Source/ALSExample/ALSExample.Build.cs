using UnrealBuildTool;

public class ALSExample : ModuleRules
{
	public ALSExample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "AIModule", "ALS", "ALSCamera"
		});
	}
}