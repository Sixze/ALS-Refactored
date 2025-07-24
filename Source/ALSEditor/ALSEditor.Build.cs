using UnrealBuildTool;

public class ALSEditor : ModuleRules
{
	public ALSEditor(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		// CppCompileWarningSettings.UnsafeTypeCastWarningLevel = WarningLevel.Warning;
		CppCompileWarningSettings.NonInlinedGenCppWarningLevel = WarningLevel.Warning;

		PublicDependencyModuleNames.AddRange([
			"Core", "CoreUObject", "Engine", "AnimGraphRuntime", "AnimationModifiers", "AnimationBlueprintLibrary", "ALS"
		]);

		if (target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange([
				"AnimGraph"
			]);

			PrivateDependencyModuleNames.AddRange([
				"BlueprintGraph"
			]);
		}
	}
}