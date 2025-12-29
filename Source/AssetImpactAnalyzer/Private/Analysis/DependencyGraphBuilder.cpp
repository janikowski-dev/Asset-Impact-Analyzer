#include "Analysis/DependencyGraphBuilder.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#include "Modules/ModuleManager.h"

void FDependencyGraphBuilder::BuildGraph(
	const TArray<FAssetData>& RootAssets,
	FDependencyGraph& OutGraph
)
{
	TSet<FName> VisitedReferencers;
	ReadAssets();

	for (const FAssetData& Asset : RootAssets)
	{
		VisitReferencers(Asset, OutGraph, VisitedReferencers);
	}
}

void FDependencyGraphBuilder::ReadAssets()
{
	AssetRegistry = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry->WaitForCompletion();
}

void FDependencyGraphBuilder::VisitReferencers(
	const FAssetData& Asset,
	FDependencyGraph& Graph,
	TSet<FName>& Visited
)
{
	const FName PackageName = Asset.PackageName;

	if (Visited.Contains(PackageName))
	{
		return;
	}

	Visited.Add(PackageName);
	Graph.AddAsset(Asset);

	for (const UE::AssetRegistry::EDependencyCategory Category :
	{
		UE::AssetRegistry::EDependencyCategory::Package,
		UE::AssetRegistry::EDependencyCategory::SearchableName
	})
	{
		TArray<FName> ReferencerPackages;

		AssetRegistry->GetReferencers(PackageName, ReferencerPackages, Category, UE::AssetRegistry::FDependencyQuery());

		for (const FName& RefPackage : ReferencerPackages)
		{
			TArray<FAssetData> RefAssets;
			AssetRegistry->GetAssetsByPackageName(RefPackage,RefAssets);

			for (const FAssetData& RefAsset : RefAssets)
			{
				if (!RefAsset.IsValid())
				{
					continue;
				}

				Graph.AddAsset(RefAsset);
				Graph.AddDependency(RefPackage,PackageName);
				VisitReferencers(RefAsset,Graph,Visited);
			}
		}
	}
}
