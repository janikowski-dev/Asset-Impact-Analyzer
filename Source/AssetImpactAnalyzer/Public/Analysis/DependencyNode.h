#pragma once

#include "AssetRegistry/AssetData.h"
#include "UObject/NameTypes.h"
#include "Containers/Set.h"

struct FDependencyNode
{
	TSet<FName> Dependencies;
	TSet<FName> Referencers;
	FAssetData Asset;

	explicit FDependencyNode(const FAssetData& InAsset) : Asset(InAsset)
	{
	}
};
