#include "GAS/Attributes/DragonAttributeSet.h"

UDragonAttributeSet::UDragonAttributeSet()
	: OverdriveEnergy(0.0f)
	, MaxOverdriveEnergy(100.0f)
	, ChargeMultiplier(1.5f)
{
}

void UDragonAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetOverdriveEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxOverdriveEnergy());
	}
}
