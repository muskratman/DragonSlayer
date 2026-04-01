#include "AI/EnemyMelee.h"
#include "GAS/Attributes/EnemyAttributeSet.h"

AEnemyMelee::AEnemyMelee()
{
	// Setup specific melee behaviors
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));
}
