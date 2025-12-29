#pragma once

#include "CoreMinimal.h"

struct FImpactAnalysisSettings
{
    bool bVerboseOutput = false;
	bool bGameOnly = true;
	bool bIncludeEngineContent = false;
	bool bIncludePluginContent = false;
	bool bIncludeWorldPartitionInternals = false;
};
