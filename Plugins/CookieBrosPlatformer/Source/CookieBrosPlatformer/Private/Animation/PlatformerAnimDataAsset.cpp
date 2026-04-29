#include "Animation/PlatformerAnimDataAsset.h"

const FPlatformerAbilityAnimEntry* UPlatformerAnimDataAsset::FindAnimEntry(const FGameplayTag& AnimTag) const
{
	if (!AnimTag.IsValid())
	{
		return nullptr;
	}

	for (const FPlatformerAbilityAnimEntry& Entry : AbilityAnimations)
	{
		if (Entry.AnimTag.MatchesTagExact(AnimTag))
		{
			return &Entry;
		}
	}

	return nullptr;
}

UAnimMontage* UPlatformerAnimDataAsset::FindMontage(const FGameplayTag& AnimTag) const
{
	if (const FPlatformerAbilityAnimEntry* Entry = FindAnimEntry(AnimTag))
	{
		return Entry->Montage;
	}

	return nullptr;
}
