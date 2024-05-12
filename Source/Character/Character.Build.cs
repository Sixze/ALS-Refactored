using UnrealBuildTool;

public class Character : ModuleRules
{
	// Character模块的构造函数，接收ReadOnlyTargetRules类型的参数Target
	public Character(ReadOnlyTargetRules Target) : base(Target)
	{
		// 设置PCH（预编译头）的使用模式
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// 设置包含顺序的版本
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

		// 启用非内联GenCpp警告
		bEnableNonInlinedGenCppWarnings = true;

		// 公共依赖
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "AIModule", "ALS", "GameplayAbilities"
		});

		// 私有依赖
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"EnhancedInput", "ALSCamera"
		});
	}
}