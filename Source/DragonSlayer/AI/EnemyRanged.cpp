#include "AI/EnemyRanged.h"
#include "GAS/Attributes/EnemyAttributeSet.h"

AEnemyRanged::AEnemyRanged()
{
	// Setup specific ranged behaviors or components if needed
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));
}
