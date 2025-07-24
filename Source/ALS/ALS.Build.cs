using UnrealBuildTool;

public class ALS : ModuleRules
{
	public ALS(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		// CppCompileWarningSettings.UnsafeTypeCastWarningLevel = WarningLevel.Warning;
		CppCompileWarningSettings.NonInlinedGenCppWarningLevel = WarningLevel.Warning;

		PublicDependencyModuleNames.AddRange([
			"Core", "CoreUObject", "Engine", "GameplayTags", "AnimGraphRuntime", "RigVM", "ControlRig"
		]);

		PrivateDependencyModuleNames.AddRange([
			"EngineSettings", "NetCore", "PhysicsCore", "Niagara"
		]);

		if (target.Type == TargetRules.TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange([
				"MessageLog"
			]);
		}

		SetupIrisSupport(target);
	}
}