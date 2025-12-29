#pragma once

#include "CoreMinimal.h"

enum class EImpactSeverity : uint8
{
	None,
	Low,
	Medium,
	High
};

struct FImpactResult
{
	TSet<FName> TransitiveReferencers;
	TSet<FName> DirectReferencers;
	TSet<FName> ImpactedAssets;
	FName RootAsset;
	int32 SeverityScore = 0;
	EImpactSeverity Severity = EImpactSeverity::None;
	TMap<FName, int32> ImpactByClass;
};