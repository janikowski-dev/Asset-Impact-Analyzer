#include "Analysis/DependencyGraph.h"

void FDependencyGraph::AddAsset(const FAssetData& Asset)
{
	const FName AssetPath = Asset.PackageName;

	if (Nodes.Contains(AssetPath))
	{
		return;
	}
	
	Nodes.Add(AssetPath, FDependencyNode(Asset));
}

void FDependencyGraph::AddDependency(
	const FName& FromAsset,
	const FName& ToAsset
)
{
	if (!Nodes.Contains(FromAsset) || !Nodes.Contains(ToAsset))
	{
		return;
	}

	Nodes[FromAsset].Dependencies.Add(ToAsset);
	Nodes[ToAsset].Referencers.Add(FromAsset);
}

const FDependencyNode* FDependencyGraph::GetNode(const FName& PackageName) const
{
	return Nodes.Find(PackageName);
}

bool FDependencyGraph::Contains(const FName& AssetPath) const
{
	return Nodes.Contains(AssetPath);
}
