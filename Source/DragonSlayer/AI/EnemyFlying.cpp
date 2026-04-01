#include "AI/EnemyFlying.h"
#include "GAS/Attributes/EnemyAttributeSet.h"

AEnemyFlying::AEnemyFlying()
{
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));
}
