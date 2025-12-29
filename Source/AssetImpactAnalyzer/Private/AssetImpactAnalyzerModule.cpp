#include "AssetImpactAnalyzerModule.h"

#include "Analysis/AssetImpactAnalyzerService.h"
#include "Analysis/DependencyGraphBuilder.h"
#include "Analysis/DependencyGraph.h"
#include "Analysis/ImpactResultJsonExporter.h"

#include "Misc/Paths.h"
#include "Misc/DateTime.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "AssetRegistry/AssetData.h"
#include "ContentBrowserModule.h"
#include "HAL/PlatformFilemanager.h"

#define LOCTEXT_NAMESPACE "FAssetImpactAnalyzerModule"

static FExecuteAction GetImpactAnalysisAction(TArray<FAssetData> SelectedAssets);

static FImpactAnalysisSettings CreateDefaultSettings();

static FDependencyGraph BuildDependencyGraph(const TArray<FAssetData>& SelectedAssets);

static void LogResult(
	const FImpactAnalysisSettings Settings,
	const FImpactResult& Result
);

static void ExportResultToFile(const FImpactResult& Result);

static const TCHAR* SeverityToString(const EImpactSeverity Severity);

void FAssetImpactAnalyzerModule::StartupModule()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	ContentBrowserModule.GetAllAssetViewContextMenuExtenders().Add(
		FContentBrowserMenuExtender_SelectedAssets::CreateRaw(
			this,
			&FAssetImpactAnalyzerModule::OnExtendAssetMenu
		)
	);
}

void FAssetImpactAnalyzerModule::ShutdownModule()
{
}

TSharedRef<FExtender> FAssetImpactAnalyzerModule::OnExtendAssetMenu(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();

	if (SelectedAssets.IsEmpty())
	{
		return Extender;
	}

	Extender->AddMenuExtension(
		"GetAssetActions",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateRaw(
			this,
			&FAssetImpactAnalyzerModule::AddAssetContextMenuEntry,
			SelectedAssets
		)
	);
	
	return Extender;
}

void FAssetImpactAnalyzerModule::AddAssetContextMenuEntry(
	FMenuBuilder& MenuBuilder,
	TArray<FAssetData> SelectedAssets
)
{
	const FExecuteAction ImpactAnalysisAction = GetImpactAnalysisAction(SelectedAssets);
	const FText ToolTip = FText::FromString("Analyze asset dependency impact");
	const FText Label = FText::FromString("Analyze Impact");
	MenuBuilder.AddMenuEntry(Label, ToolTip, FSlateIcon(), ImpactAnalysisAction);
}

static FExecuteAction GetImpactAnalysisAction(TArray<FAssetData> SelectedAssets)
{
	return FExecuteAction::CreateLambda([SelectedAssets]
	{
		const FImpactAnalysisSettings Settings = CreateDefaultSettings();
		const FDependencyGraph Graph = BuildDependencyGraph(SelectedAssets);
		FAssetImpactAnalyzerService Analyzer;

		for (const FAssetData& Asset : SelectedAssets)
		{
			const FImpactResult Result = Analyzer.AnalyzeImpact(Asset, Graph, Settings);
			LogResult(Settings, Result);
			ExportResultToFile(Result);
		}
	});
}


static FImpactAnalysisSettings CreateDefaultSettings()
{
	FImpactAnalysisSettings Settings;
	Settings.bVerboseOutput = false;
	Settings.bGameOnly = true;
	Settings.bIncludePluginContent = false;
	Settings.bIncludeEngineContent = false;
	Settings.bIncludeWorldPartitionInternals = false;
	return Settings;
}

static FDependencyGraph BuildDependencyGraph(const TArray<FAssetData>& SelectedAssets)
{
	FDependencyGraph Graph;
	FDependencyGraphBuilder Builder;
	Builder.BuildGraph(SelectedAssets, Graph);
	return Graph;
}

static void LogResult(const FImpactAnalysisSettings Settings, const FImpactResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("==================================="));
	UE_LOG(LogTemp, Warning, TEXT("Impact analysis for: %s"), *Result.RootAsset.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Severity: %s (score %d)"), SeverityToString(Result.Severity), Result.SeverityScore);

	for (const TPair<FName, int32>& Pair : Result.ImpactByClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("  %s: %d"), * Pair.Key.ToString(), Pair.Value);
	}

	if (Settings.bVerboseOutput || Result.Severity == EImpactSeverity::High)
	{
		UE_LOG(LogTemp, Warning, TEXT("Affected assets:"));

		for (const FName& ImpactedAsset : Result.ImpactedAssets)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - %s"), * ImpactedAsset.ToString());
		}
	}
				
	UE_LOG(LogTemp, Warning, TEXT("==================================="));
}

static void ExportResultToFile(const FImpactResult& Result)
{
	const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d_%H-%M-%S"));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const FString OutputDir = FPaths::ProjectSavedDir() / TEXT("Analysis");

	if (!PlatformFile.DirectoryExists(*OutputDir))
	{
		PlatformFile.CreateDirectoryTree(*OutputDir);
	}
	
	FString Name = Result.RootAsset.ToString();
	int32 SlashIndex;
	
	if (Name.FindLastChar(TEXT('/'), SlashIndex))
	{
		Name = Name.Mid(SlashIndex + 1);
	}

	Name.ReplaceInline(TEXT("."), TEXT("_"));
				
	const FString FileName = FString::Printf(TEXT("AssetImpact_%s_%s.json"), *Name, *Timestamp);
	const FString OutputPath = OutputDir / FileName;
	FImpactResultJsonExporter::ExportToFile(Result, OutputPath);
}

static const TCHAR* SeverityToString(const EImpactSeverity Severity)
{
	switch (Severity)
	{
	case EImpactSeverity::Low:
		return TEXT("Low");

	case EImpactSeverity::Medium:
		return TEXT("Medium");

	case EImpactSeverity::High:
		return TEXT("High");

	default:
		return TEXT("None");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(
	FAssetImpactAnalyzerModule,
	AssetImpactAnalyzer
)
