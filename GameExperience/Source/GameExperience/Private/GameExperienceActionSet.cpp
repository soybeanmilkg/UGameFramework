// Fill out your copyright notice in the Description page of Project Settings.

#include "GameExperienceActionSet.h"

#include "GameFeatureAction.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "GameExperience"

UGameExperienceActionSet::UGameExperienceActionSet()
{
}

#if WITH_EDITOR
EDataValidationResult UGameExperienceActionSet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			const EDataValidationResult ChildResult = Action->IsDataValid(Context);
			Result = CombineDataValidationResults(Result, ChildResult);
		}
		else
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("ActionEntryIsNull", "Null entry at index {0} in Actions"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

#if WITH_EDITORONLY_DATA
void UGameExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (TObjectPtr<UGameFeatureAction>& Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif

#undef LOCTEXT_NAMESPACE
