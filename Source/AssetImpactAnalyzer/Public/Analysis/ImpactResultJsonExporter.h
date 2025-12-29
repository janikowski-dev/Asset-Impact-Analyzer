#pragma once

#include "Analysis/ImpactResult.h"

class FImpactResultJsonExporter
{
public:
	static bool ExportToFile(
		const FImpactResult& Result,
		const FString& FilePath
	);
};
