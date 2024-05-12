using UnrealBuildTool;

public class ALS : ModuleRules
{
	public ALS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

		bEnableNonInlinedGenCppWarnings = true;
		UnsafeTypeCastWarningLevel = WarningLevel.Warning;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "GameplayTags", "AnimGraphRuntime", "RigVM", "ControlRig", "GameplayAbilities"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"EngineSettings", "NetCore", "PhysicsCore", "Niagara", "GameplayAbilities", "ALSCamera", "EnhancedInput"
		});

		if (Target.Type == TargetRules.TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(new[]
			{
				"MessageLog"
			});
		}

		SetupIrisSupport(Target);
	}
}