#pragma once

#include "DependencyNode.h"

#include "UObject/NameTypes.h"
#include "Containers/Map.h"

class FDependencyGraph
{
public:
	void AddAsset(const FAssetData& Asset);

	void AddDependency(
		const FName& FromAsset,
		const FName& ToAsset
	);

	bool Contains(const FName& AssetPath) const;

	const FDependencyNode* GetNode(const FName& PackageName) const;

private:
	TMap<FName, FDependencyNode> Nodes;
};
