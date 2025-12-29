#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetData.h"

class FExtender;
class FMenuBuilder;

class FAssetImpactAnalyzerModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void AddAssetContextMenuEntry(
		FMenuBuilder& MenuBuilder,
		TArray<FAssetData> SelectedAssets
	);

	TSharedRef<FExtender> OnExtendAssetMenu(
		const TArray<FAssetData>& SelectedAssets
	);
};
