#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/PlatformerCharacterAttributeSet.h"
#include "DragonAttributeSet.generated.h"

/**
 * UDragonAttributeSet
 * Health, Overdrive, and Combat stats for the Dragon character.
 */
UCLASS()
class DRAGONSLAYER_API UDragonAttributeSet : public UPlatformerCharacterAttributeSet
{
	GENERATED_BODY()

public:
	UDragonAttributeSet();

	// Overdrive Resource
	UPROPERTY(BlueprintReadOnly, Category="Overdrive")
	FGameplayAttributeData OverdriveEnergy;
	PLATFORMER_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, OverdriveEnergy)

	UPROPERTY(BlueprintReadOnly, Category="Overdrive")
	FGameplayAttributeData MaxOverdriveEnergy;
	PLATFORMER_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, MaxOverdriveEnergy)

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	FGameplayAttributeData ChargeMultiplier;
	PLATFORMER_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, ChargeMultiplier)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};
