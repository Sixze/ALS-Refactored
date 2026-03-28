using UnrealBuildTool;

public class ALSExtras : ModuleRules
{
	public ALSExtras(ReadOnlyTargetRules target) : base(target)
	{
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

		// if (Target.bBuildEditor)
		// {
		// 	// Verify that all source files include all required dependencies.
		//
		// 	bUseUnity = false;
		// 	PCHUsage = PCHUsageMode.NoPCHs;
		// }

		// CppCompileWarningSettings.UnsafeTypeCastWarningLevel = WarningLevel.Warning;
		CppCompileWarningSettings.NonInlinedGenCppWarningLevel = WarningLevel.Warning;

		PublicDependencyModuleNames.AddRange([
			"Core", "CoreUObject", "Engine", "AIModule", "ALS"
		]);

		PrivateDependencyModuleNames.AddRange([
			"EnhancedInput", "ALSCamera"
		]);
	}
}