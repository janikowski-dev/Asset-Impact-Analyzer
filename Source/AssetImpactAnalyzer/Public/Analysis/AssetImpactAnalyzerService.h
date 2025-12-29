#pragma once

#include "Analysis/ImpactAnalysisSettings.h"
#include "DependencyGraph.h"
#include "ImpactResult.h"

#include "AssetRegistry/AssetData.h"

class FAssetImpactAnalyzerService
{
public:
	FImpactResult AnalyzeImpact(
		const FAssetData& RootAsset,
		const FDependencyGraph& Graph,
		const FImpactAnalysisSettings& Settings
	);
};
