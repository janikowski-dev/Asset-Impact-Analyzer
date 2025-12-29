#pragma once

#include "DependencyGraph.h"

#include "AssetRegistry/IAssetRegistry.h"

class FDependencyGraphBuilder
{
public:
    void BuildGraph(
        const TArray<FAssetData>& RootAssets,
        FDependencyGraph& OutGraph
    );

private:
    void ReadAssets();
    
    void VisitAsset(
        const FAssetData& Asset,
        FDependencyGraph& Graph,
        TSet<FName>& Visited
    );

    void VisitReferencers(
        const FAssetData& Asset,
        FDependencyGraph& Graph,
        TSet<FName>& Visited
    );

private:
    IAssetRegistry *AssetRegistry = nullptr;
};
