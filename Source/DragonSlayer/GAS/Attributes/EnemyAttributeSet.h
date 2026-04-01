#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/PlatformerCharacterAttributeSet.h"
#include "EnemyAttributeSet.generated.h"

/**
 * UEnemyAttributeSet
 * Simplified stat sheet for hostile characters. Health and Damage only.
 */
UCLASS()
class DRAGONSLAYER_API UEnemyAttributeSet : public UPlatformerCharacterAttributeSet
{
	GENERATED_BODY()
	
public:
	UEnemyAttributeSet();
};
