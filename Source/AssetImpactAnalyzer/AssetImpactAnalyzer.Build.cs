using UnrealBuildTool;

public class AssetImpactAnalyzer : ModuleRules
{
	public AssetImpactAnalyzer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange([
			"Core",
			"CoreUObject",
			"Engine",
			"AssetRegistry"
		]);

		PrivateDependencyModuleNames.AddRange([
			"Slate",
			"SlateCore",
			"EditorStyle",
			"ToolMenus",
			"ContentBrowser",
			"Projects",
			"Json"
		]);
	}
}