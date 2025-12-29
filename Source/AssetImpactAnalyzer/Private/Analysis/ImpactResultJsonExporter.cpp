#include "Analysis/ImpactResultJsonExporter.h"

#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "CoreMinimal.h"

static void AppendBase(const FImpactResult& Result, const TSharedPtr<FJsonObject> Root);

static void AppendGroups(const FImpactResult& Result, const TSharedPtr<FJsonObject> Root);

static void AppendPaths(const FImpactResult& Result, const TSharedPtr<FJsonObject> Root);

static bool SaveToFile(const FString& FilePath, const TSharedPtr<FJsonObject> Root);

static FString SeverityToString(const EImpactSeverity Severity);

bool FImpactResultJsonExporter::ExportToFile(
	const FImpactResult& Result,
	const FString& FilePath
)
{
	const TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
	AppendBase(Result, Root);
	AppendGroups(Result, Root);
	AppendPaths(Result, Root);
	return SaveToFile(FilePath, Root);
}

static void AppendBase(const FImpactResult& Result, const TSharedPtr<FJsonObject> Root)
{
	Root->SetStringField(TEXT("RootAsset"), Result.RootAsset.ToString());
	Root->SetStringField(TEXT("Severity"), SeverityToString(Result.Severity));
	Root->SetNumberField(TEXT("Score"), Result.SeverityScore);
}

static void AppendGroups(const FImpactResult& Result, const TSharedPtr<FJsonObject> Root)
{
	const TSharedPtr<FJsonObject> Groups = MakeShared<FJsonObject>();
	for (const TPair<FName, int32>& Pair : Result.ImpactByClass)
	{
		Groups->SetNumberField(Pair.Key.ToString(), Pair.Value);
	}
	Root->SetObjectField(TEXT("ImpactByClass"), Groups);
}

static void AppendPaths(const FImpactResult& Result, const TSharedPtr<FJsonObject> Root)
{
	TArray<TSharedPtr<FJsonValue>> Paths;
	for (const FName& ImpactedAsset : Result.ImpactedAssets)
	{
		Paths.Add(MakeShared<FJsonValueString>(ImpactedAsset.ToString()));
	}
	Root->SetArrayField(TEXT("ImpactedAssets"), Paths);
}

static bool SaveToFile(const FString& FilePath, const TSharedPtr<FJsonObject> Root)
{
	FString Output;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);

	if (!FJsonSerializer::Serialize(Root.ToSharedRef(), Writer))
	{
		return false;
	}

	return FFileHelper::SaveStringToFile(Output, *FilePath);
}

static FString SeverityToString(const EImpactSeverity Severity)
{
	switch (Severity)
	{
	case EImpactSeverity::Low:
		return TEXT("Low");

	case EImpactSeverity::Medium:
		return TEXT("Medium");

	case EImpactSeverity::High:
		return TEXT("High");

	default:
		return TEXT("None");
	}
}
