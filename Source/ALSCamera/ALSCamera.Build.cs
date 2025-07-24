using UnrealBuildTool;

public class ALSCamera : ModuleRules
{
	public ALSCamera(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		// CppCompileWarningSettings.UnsafeTypeCastWarningLevel = WarningLevel.Warning;
		CppCompileWarningSettings.NonInlinedGenCppWarningLevel = WarningLevel.Warning;

		PublicDependencyModuleNames.AddRange([
			"Core", "CoreUObject", "Engine", "GameplayTags", "ALS"
		]);

		PrivateDependencyModuleNames.AddRange([
			"EngineSettings"
		]);
	}
}