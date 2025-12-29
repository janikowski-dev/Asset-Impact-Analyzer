#include "Analysis/AssetImpactAnalyzerService.h"
#include "Analysis/DependencyGraph.h"

#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "Containers/Queue.h"

namespace AssetImpactAnalyzerServiceConstants
{
	const TMap<FName, FName> GImpact_Class_By_Asset_Class =
	{
		{ TEXT("World"), TEXT("Level") },
		{ TEXT("Blueprint"), TEXT("Blueprint") },
		{ TEXT("StaticMesh"), TEXT("Mesh") },
		{ TEXT("SkeletalMesh"), TEXT("Mesh") },
		{ TEXT("Material"), TEXT("Material") },
		{ TEXT("MaterialInstance"), TEXT("Material") },
		{ TEXT("Texture2D"), TEXT("Texture") }
	};

	const TMap<FName, int32> GImpact_Score_By_Class =
	{
		{ TEXT("World"), 5 },
		{ TEXT("Blueprint"), 4 },
		{ TEXT("StaticMesh"), 3 },
		{ TEXT("SkeletalMesh"), 3 },
		{ TEXT("Material"), 2 },
		{ TEXT("MaterialInstance"), 2 },
		{ TEXT("Texture2D"), 1 }
	};
	
	static const FName DefaultImpactClass = TEXT("Other");
	
	constexpr int32 GSeverity_Medium_Max = 9;
	constexpr int32 GSeverity_Low_Max = 4;
	constexpr int32 GSeverity_None = 0;
	constexpr int32 GDefault_Score = 1;
}

using namespace AssetImpactAnalyzerServiceConstants;

static void FillImpactValues(const FDependencyGraph& Graph, FImpactResult& Result);

static bool PassesFilter(
	const FName& PackageName,
	const FImpactAnalysisSettings& Settings
);

static int32 ScoreAsset(const FAssetData& Asset);

static FName GetImpactClass(const FAssetData& Asset);

FImpactResult FAssetImpactAnalyzerService::AnalyzeImpact(
	const FAssetData& RootAsset,
	const FDependencyGraph& Graph,
	const FImpactAnalysisSettings& Settings
)
{
	FImpactResult Result;
	Result.RootAsset = RootAsset.PackageName;

	if (!PassesFilter(Result.RootAsset, Settings))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AssetImpactAnalyzer] Root asset excluded by policy: %s"), *Result.RootAsset.ToString());
		return Result;
	}

	const FDependencyNode* RootNode = Graph.GetNode(RootAsset.PackageName);

	if (!RootNode)
	{
		return Result;
	}

	TQueue<FName> Queue;

	for (const FName& Ref : RootNode->Referencers)
	{
		if (!PassesFilter(Ref, Settings))
		{
			continue;
		}

		Result.DirectReferencers.Add(Ref);
		Result.ImpactedAssets.Add(Ref);
		Queue.Enqueue(Ref);
	}

	while (!Queue.IsEmpty())
	{
		FName Current;
		Queue.Dequeue(Current);

		const FDependencyNode* Node = Graph.GetNode(Current);
		
		if (!Node)
		{
			continue;
		}

		for (const FName& Ref : Node->Referencers)
		{
			if (!PassesFilter(Ref, Settings))
			{
				continue;
			}

			if (Result.ImpactedAssets.Contains(Ref))
			{
				continue;
			}

			Result.TransitiveReferencers.Add(Ref);
			Result.ImpactedAssets.Add(Ref);
			Queue.Enqueue(Ref);
		}
	}

	FillImpactValues(Graph, Result);
	return Result;
}

static bool PassesFilter(
	const FName& PackageName,
	const FImpactAnalysisSettings& Settings
)
{
	const FString Path = PackageName.ToString();

	if (!Settings.bIncludeWorldPartitionInternals && (Path.Contains(TEXT("__ExternalActors__")) || Path.Contains(TEXT("__ExternalObjects__"))))
	{
		return false;
	}

	if (Settings.bGameOnly)
	{
		return Path.StartsWith(TEXT("/Game"));
	}

	if (!Settings.bIncludeEngineContent && Path.StartsWith(TEXT("/Engine")))
	{
		return false;
	}

	if (!Settings.bIncludePluginContent && !Path.StartsWith(TEXT("/Game")) && !Path.StartsWith(TEXT("/Engine")))
	{
		return false;
	}

	return true;
}

static void FillImpactValues(const FDependencyGraph& Graph, FImpactResult& Result)
{
	int32 Score = 0;
	
	for (const FName& Package : Result.ImpactedAssets)
	{
		const FDependencyNode* Node = Graph.GetNode(Package);
		
		if (!Node)
		{
			continue;
		}

		const FAssetData& Asset = Node->Asset;
		Score += ScoreAsset(Asset);

		const FName ImpactClass = GetImpactClass(Asset);
		Result.ImpactByClass.FindOrAdd(ImpactClass)++;
	}

	Result.SeverityScore = Score;

	if (Score == GSeverity_None)
	{
		Result.Severity = EImpactSeverity::None;
	}
	else if (Score <= GSeverity_Low_Max)
	{
		Result.Severity = EImpactSeverity::Low;
	}
	else if (Score <= GSeverity_Medium_Max)
	{
		Result.Severity = EImpactSeverity::Medium;
	}
	else
	{
		Result.Severity = EImpactSeverity::High;
	}
}

static FName GetImpactClass(const FAssetData& Asset)
{
	const FName ClassName = Asset.AssetClassPath.GetAssetName();

	if (const FName* Class = GImpact_Class_By_Asset_Class.Find(ClassName))
	{
		return *Class;
	}

	return DefaultImpactClass;
}

static int32 ScoreAsset(const FAssetData& Asset)
{
	const FName ClassName = Asset.AssetClassPath.GetAssetName();

	if (const int32* Score = GImpact_Score_By_Class.Find(ClassName))
	{
		return *Score;
	}

	return GDefault_Score;
}